/**
 * @file    rcc_conf.h
 * @brief   System clock configuration for the STM32F103C8T6.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * Provides the 72 MHz clock-tree initialisation used by the metronome. The
 * function is intentionally minimal and register-level; it does not depend on
 * the STM32 HAL or the Standard Peripheral Library.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef RCC_CONF_H
#define RCC_CONF_H

#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @brief   Configure the STM32F103 clock tree for 72 MHz SYSCLK.
 *
 * @details
 * Clock tree produced by this function:
 * - HSE oscillator: 8 MHz.
 * - PLL source: HSE, multiplier x9 -> 72 MHz.
 * - SYSCLK: PLL output, 72 MHz.
 * - AHB: /1 -> 72 MHz.
 * - APB2: /1 -> 72 MHz.
 * - APB1: /2 -> 36 MHz (maximum permitted for APB1).
 * - Flash: 2 wait states, prefetch buffer enabled.
 *
 * @note
 * The function blocks until the HSE and PLL ready flags are set. It must be
 * called before @ref systick_init() because SysTick relies on the final core
 * frequency.
 *
 * @warning
 * The HSE and PLL startup loops are intentionally unbounded: the SysTick time
 * base is not yet available at this point. A failed crystal therefore results
 * in a deterministic hang rather than a reset loop. The watchdog is not yet
 * active during clock start-up.
 */
void clock_init72mhz(void);

#endif /* RCC_CONF_H */
