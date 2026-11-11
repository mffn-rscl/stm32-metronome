/**
 * @file    i2c_driver.h
 * @brief   Polled I2C1 master driver with deterministic timeouts.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * Minimal I2C1 master implementation for the SSD1306 OLED. The driver exposes
 * primitive start/address/data/stop operations so that the display layer can
 * build complete transactions without depending on a generic I2C abstraction.
 *
 * @note
 * The bus is configured for 100 kHz standard mode from a 36 MHz APB1 clock.
 * PB6 is SCL and PB7 is SDA; both pins use alternate-function open-drain mode
 * and require external pull-up resistors (330 ohm on the metronome board).
 *
 * @warning
 * The driver is not re-entrant and does not implement clock stretching
 * recovery, arbitration loss handling or multi-master support. It is intended
 * for a single-master bus with one SSD1306 slave.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef I2C1_DRIVER_H
#define I2C1_DRIVER_H

#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"
#include "../Inc/timeout_handler.h"

/**
 * @def     GPIOB_I2C1_MASK
 * @brief   GPIO configuration nibble for PB6/PB7 in alternate-function
 *          open-drain 50 MHz mode.
 */
#define GPIOB_I2C1_MASK 0xFU

/**
 * @def     I2C1_APB1_FREQUENCY
 * @brief   APB1 peripheral clock in megahertz, written to @c I2C1->CR2.
 */
#define I2C1_APB1_FREQUENCY 36U

/**
 * @def     I2C1_APB1_FREQUENCY_DIVIDER
 * @brief   Clock-control value producing 100 kHz SCL.
 *
 * @details
 * @c CCR = 180 with @c CR2 = 36 MHz gives
 * SCL = 36 MHz / (2 * 180) = 100 kHz in standard mode.
 */
#define I2C1_APB1_FREQUENCY_DIVIDER 180U

/**
 * @def     I2C1_TRISE_VAL
 * @brief   Maximum SCL rise time in peripheral clock cycles plus one.
 *
 * @details
 * For 1000 ns maximum rise time at 36 MHz: 36 * 1000 ns + 1 = 37.
 */
#define I2C1_TRISE_VAL 37U

/**
 * @def     I2C1_TIMEOUT_MS
 * @brief   Maximum time to wait for any I2C event, in milliseconds.
 */
#define I2C1_TIMEOUT_MS 40U

/**
 * @def     I2C1_EXPECTED_READY_VAL
 * @brief   Expected masked value when waiting for a set event flag.
 */
#define I2C1_EXPECTED_READY_VAL 1U

/**
 * @brief   Initialise I2C1 and the PB6/PB7 pins.
 *
 * @note
 * Enables the I2C1 and GPIOB clocks, configures PB6/PB7 as alternate-function
 * open-drain, programs the 100 kHz timing registers and enables the peripheral.
 * DMA requests are enabled for the transmit path used by the display driver.
 *
 * @warning
 * Must be called after @ref clock_init72mhz() and @ref systick_init(). The
 * function performs blocking register writes but no bus transaction.
 */
void i2c1_init(void);

/**
 * @brief   Generate an I2C START condition and wait for the SB flag.
 *
 * @note
 * Blocks for at most @ref I2C1_TIMEOUT_MS. On timeout the system is reset by
 * @ref timeout_sleep().
 *
 * @warning
 * Not re-entrant. Do not call from an interrupt while a foreground transaction
 * is in progress.
 */
void i2c1_start(void);

/**
 * @brief   Transmit a 7-bit slave address in write mode and wait for ADDR.
 *
 * @param[in] addr 7-bit slave address (for example 0x3C for the SSD1306).
 *
 * @note
 * The address is shifted left by one; the R/W bit is always 0 (write).
 *
 * @note
 * If the address is not acknowledged (AF set), the function clears AF, issues
 * STOP and returns without reporting an error to the caller.
 *
 * @warning
 * Blocking with a bounded timeout; a timeout causes a system reset.
 */
void i2c1_slave_adress(uint8_t addr);

/**
 * @brief   Write one data byte and wait for TXE.
 *
 * @param[in] data Byte to place in the I2C data register.
 *
 * @note
 * TXE indicates that the data register is empty and the next byte may be
 * written. It does not indicate that the byte has been acknowledged; the
 * transaction is terminated by @ref i2c1_stop_condition(), which waits for BTF.
 *
 * @warning
 * Blocking with a bounded timeout; a timeout causes a system reset.
 */
void i2c1_data(uint8_t data);

/**
 * @brief   Wait for byte transfer completion and generate a STOP condition.
 *
 * @note
 * Waits for BTF (byte transfer finished) or AF (acknowledge failure), then
 * issues STOP. A pending AF flag is cleared before returning.
 *
 * @warning
 * Blocking with a bounded timeout; a timeout causes a system reset.
 */
void i2c1_stop_condition(void);

#endif /* I2C1_DRIVER_H */
