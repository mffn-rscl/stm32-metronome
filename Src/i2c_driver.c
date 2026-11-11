/**
 * @file    i2c_driver.c
 * @brief   Polled I2C1 master implementation for the SSD1306 display.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The driver uses the STM32F1 I2C peripheral in master transmitter mode. Each
 * public operation advances one step of a transaction and waits for the
 * corresponding event flag with a bounded timeout. The display driver sequences
 * these operations to form complete command and data writes.
 *
 * @note
 * The I2C1 event interrupt is not used; the DMA channel is used only for the
 * bulk frame-buffer upload and generates a STOP from its transfer-complete
 * interrupt.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/i2c_driver.h"

/**
 * @brief   Initialise I2C1 and configure PB6/PB7.
 *
 * @note
 * The GPIO mode nibble 0xF selects output mode 11 (50 MHz) and alternate
 * function open-drain configuration 11, as required for I2C.
 *
 * @note
 * DMAEN is set once. The previous implementation wrote the same bit twice;
 * the duplicate write has been removed during documentation cleanup.
 *
 * @warning
 * The peripheral must be disabled before programming @c CR2, @c CCR and
 * @c TRISE. The function honours that requirement.
 */
void i2c1_init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

  /* PB6 = SCL, PB7 = SDA: 50 MHz alternate-function open-drain. */
  GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_MODE7 |
                  GPIO_CRL_CNF6  | GPIO_CRL_CNF7);
  GPIOB->CRL |= (GPIOB_I2C1_MASK << GPIO_CRL_MODE6_Pos) |
                (GPIOB_I2C1_MASK << GPIO_CRL_MODE7_Pos);

  I2C1->CR1 &= ~I2C_CR1_PE;

  I2C1->CR2  = I2C1_APB1_FREQUENCY;
  I2C1->CCR  = I2C1_APB1_FREQUENCY_DIVIDER;
  I2C1->TRISE = I2C1_TRISE_VAL;
  I2C1->CR2 |= I2C_CR2_DMAEN;

  I2C1->CR1 |= I2C_CR1_PE;
}

/**
 * @brief   Generate START and wait for the start-bit flag.
 *
 * @note
 * On timeout, @ref timeout_sleep() resets the MCU.
 */
void i2c1_start(void)
{
  I2C1->CR1 |= I2C_CR1_START;

  timeout_sleep(
      system_wait_for_flag(
          &I2C1->SR1, I2C_SR1_SB,
          I2C1_EXPECTED_READY_VAL, I2C1_TIMEOUT_MS));
}

/**
 * @brief   Send a 7-bit address in write mode and wait for ADDR or AF.
 *
 * @note
 * A repeated START is not generated. The caller must issue START before each
 * address phase.
 *
 * @note
 * Reading SR1 followed by SR2 clears the ADDR flag and releases SCL.
 *
 * @warning
 * If AF is set the function clears the flag and issues STOP, then returns. The
 * caller cannot distinguish this condition from a successful address phase;
 * this is a known limitation of the current API.
 */
void i2c1_slave_adress(uint8_t addr)
{
  I2C1->DR = (uint8_t)(addr << 1U);

  timeout_sleep(
      system_wait_for_flag(
          &I2C1->SR1, (I2C_SR1_ADDR | I2C_SR1_AF),
          I2C1_EXPECTED_READY_VAL, I2C1_TIMEOUT_MS));

  if ((I2C1->SR1 & I2C_SR1_AF) != 0U)
  {
    I2C1->SR1 &= ~I2C_SR1_AF;
    I2C1->CR1 |= I2C_CR1_STOP;
    return;
  }

  /* Read SR1 then SR2 to clear ADDR and start the data phase. */
  (void)I2C1->SR1;
  (void)I2C1->SR2;
}

/**
 * @brief   Write one byte and wait until the data register is empty.
 *
 * @note
 * TXE is asserted when @c DR is empty. The final byte is flushed by
 * @ref i2c1_stop_condition(), which waits for BTF.
 */
void i2c1_data(uint8_t data)
{
  I2C1->DR = data;

  timeout_sleep(
      system_wait_for_flag(
          &I2C1->SR1, I2C_SR1_TXE,
          I2C1_EXPECTED_READY_VAL, I2C1_TIMEOUT_MS));
}

/**
 * @brief   Wait for BTF or AF and generate STOP.
 *
 * @note
 * BTF guarantees that the last byte has been shifted out before STOP is
 * requested. If AF is pending it is cleared after STOP is issued.
 */
void i2c1_stop_condition(void)
{
  timeout_sleep(
      system_wait_for_flag(
          &I2C1->SR1, (I2C_SR1_BTF | I2C_SR1_AF),
          I2C1_EXPECTED_READY_VAL, I2C1_TIMEOUT_MS));

  I2C1->CR1 |= I2C_CR1_STOP;

  if ((I2C1->SR1 & I2C_SR1_AF) != 0U)
  {
    I2C1->SR1 &= ~I2C_SR1_AF;
  }
}
