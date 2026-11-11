/**
 * @file    dma_driver.h
 * @brief   DMA1 Channel 6 driver for I2C1 transmit transfers.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * DMA1 Channel 6 is hard-wired to the I2C1 transmit request on the
 * STM32F103C8T6. The driver configures a memory-to-peripheral transfer from the
 * SSD1306 frame buffer to @c I2C1->DR and terminates the I2C transaction from
 * the DMA transfer-complete interrupt.
 *
 * @note
 * The DMA controller performs byte transfers with memory increment enabled.
 * The frame buffer must remain valid for the entire transfer; it is declared
 * @c volatile and statically allocated by the display module.
 *
 * @warning
 * Only one transfer may be active at a time. Starting a new transfer while the
 * previous one is in flight results in data corruption.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @brief   Enable DMA1 Channel 6.
 *
 * @note
 * Setting @c EN starts the transfer previously programmed by
 * @ref dma_channel6_set_pma(). The channel must be disabled before the memory
 * address and transfer count are changed.
 */
void dma_channel6_enable(void);

/**
 * @brief   Disable DMA1 Channel 6.
 *
 * @note
 * Disabling the channel aborts an in-progress transfer. The I2C STOP condition
 * is not generated automatically when the channel is disabled manually.
 */
void dma_channel6_disable(void);

/**
 * @brief   Initialise DMA1 Channel 6 for I2C1 TX.
 *
 * @note
 * Enables the DMA1 clock, selects memory-increment and memory-to-peripheral
 * direction, sets @c I2C1->DR as the peripheral address, enables transfer-error
 * and transfer-complete interrupts, and configures the NVIC.
 *
 * @note
 * The channel is left disabled. Call @ref dma_channel6_set_pma() and then
 * @ref dma_channel6_enable() to start a transfer.
 */
void dma_channel6_init(void);

/**
 * @brief   Program the source address and transfer size.
 *
 * @param[in] framebuffer Pointer to the source buffer in SRAM.
 * @param[in] size        Number of bytes to transfer.
 *
 * @note
 * The channel is disabled before @c CMAR and @c CNDTR are written, as required
 * by the STM32F1 DMA programming model.
 *
 * @warning
 * @p framebuffer must remain valid until the transfer-complete interrupt fires.
 * Passing a stack pointer results in undefined behaviour.
 */
void dma_channel6_set_pma(uint8_t *framebuffer, uint16_t size);

/**
 * @brief   DMA1 Channel 6 interrupt handler.
 *
 * @note
 * ISR context. Handles transfer complete and transfer error. Both paths clear
 * the corresponding interrupt flag, disable the channel and generate an I2C
 * STOP condition to terminate the display transaction.
 *
 * @warning
 * The handler calls @ref i2c1_stop_condition(), which is a blocking,
 * timeout-protected function. Keep the I2C timeout short and do not call the
 * display driver from a higher-priority interrupt.
 */
void DMA1_chan6_IRQ_handler(void);

#endif /* DMA_DRIVER_H */
