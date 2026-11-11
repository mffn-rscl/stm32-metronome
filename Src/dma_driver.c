/**
 * @file    dma_driver.c
 * @brief   DMA1 Channel 6 implementation for SSD1306 frame-buffer uploads.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The driver configures DMA1 Channel 6 in memory-to-peripheral mode with byte
 * granularity and memory address increment. The peripheral address is fixed at
 * @c I2C1->DR. Transfer completion is signalled to the display layer by
 * generating an I2C STOP condition.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/dma_driver.h"
#include "../Inc/i2c_driver.h"

/**
 * @brief   Enable DMA1 Channel 6.
 *
 * @note
 * The channel must have been programmed with a valid source address and count
 * before this call.
 */
void dma_channel6_enable(void)
{
  DMA1_Channel6->CCR |= DMA_CCR_EN;
}

/**
 * @brief   Disable DMA1 Channel 6.
 *
 * @note
 * A manual disable does not generate an I2C STOP condition.
 */
void dma_channel6_disable(void)
{
  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
}

/**
 * @brief   Program the DMA source address and transfer count.
 *
 * @note
 * @c CMAR holds the memory address and @c CNDTR holds the remaining byte count.
 * The channel is disabled first to satisfy the STM32F1 DMA configuration
 * requirements.
 */
void dma_channel6_set_pma(uint8_t *framebuffer, uint16_t size)
{
  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
  DMA1_Channel6->CMAR  = (uint32_t)framebuffer;
  DMA1_Channel6->CNDTR = size;
}

/**
 * @brief   Initialise DMA1 Channel 6 for I2C1 transmit.
 *
 * @note
 * @c DIR = 1 selects memory-to-peripheral. @c MINC = 1 increments the memory
 * address after each byte. The peripheral address is fixed, so @c PINC is left
 * cleared.
 *
 * @note
 * NVIC priority 1 is used for the DMA completion interrupt. The TIM2 beat
 * interrupt uses priority 0 and can therefore pre-empt the display transfer.
 */
void dma_channel6_init(void)
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
  DMA1_Channel6->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;

  DMA1_Channel6->CPAR = (uint32_t)&I2C1->DR;

  /* Enable transfer-error and transfer-complete interrupts. */
  DMA1_Channel6->CCR |= DMA_CCR_TEIE | DMA_CCR_TCIE;

  NVIC_SetPriority(DMA1_Channel6_IRQn, 1);
  NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

/**
 * @brief   DMA1 Channel 6 interrupt handler.
 *
 * @note
 * ISR context. Transfer complete and transfer error are handled identically:
 * the flag is cleared, the channel is disabled and an I2C STOP is issued. This
 * keeps the display transaction bounded even when a bus error occurs.
 *
 * @warning
 * The I2C STOP path polls @c SR1 and can therefore block for up to
 * @ref I2C1_TIMEOUT_MS inside the interrupt. A timeout triggers a system reset.
 */
void DMA1_chan6_IRQ_handler(void)
{
  if ((DMA1->ISR & DMA_ISR_TCIF6) != 0U)
  {
    DMA1->IFCR = DMA_IFCR_CTCIF6;
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
   
  I2C1->CR1 |= I2C_CR1_STOP;

  }

  if ((DMA1->ISR & DMA_ISR_TEIF6) != 0U)
  {
    DMA1->IFCR = DMA_IFCR_CTEIF6;
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    
    I2C1->CR1 |= I2C_CR1_STOP;

  }
}
