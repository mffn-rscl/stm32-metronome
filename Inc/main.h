/**
 * @file    main.h
 * @brief   Application-wide configuration and common include aggregation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * This header is the single entry point for application-level configuration.
 * It aggregates the CMSIS device definition and every project driver header so
 * that translation units do not need to duplicate include lists.
 *
 * @note
 * The project intentionally targets the STM32F103C8T6 register interface
 * directly (CMSIS only). No STM32 HAL or Standard Peripheral Library is used.
 *
 * @warning
 * Include this header only from application code. CMSIS device headers must
 * not be modified to add project-specific configuration.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef VVC_MAIN_H
#define VVC_MAIN_H

#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"

#include "../Inc/i2c_driver.h"
#include "../Inc/rcc_conf.h"
#include "../Inc/ssd1306.h"
#include "../Inc/dma_driver.h"
#include "../Inc/tim2_bpm_clock_control.h"

#include "../Inc/timeout_handler.h"
#include "../Inc/pwm_buzzer_driver.h"
#include "../Inc/IWDG.h"

/**
 * @def     CPU_FREQ_HZ
 * @brief   Core clock frequency in hertz after @ref clock_init72mhz().
 *
 * @details
 * The STM32F103C8T6 is configured for a 72 MHz SYSCLK derived from the 8 MHz
 * HSE through the PLL (x9). APB1 runs at 36 MHz and APB2 at 72 MHz.
 *
 * @note
 * This value is consumed by @ref systick_init(). It must match the actual
 * hardware clock tree; otherwise all millisecond timeouts are scaled
 * incorrectly.
 */
#define CPU_FREQ_HZ 72000000U

#endif /* VVC_MAIN_H */
