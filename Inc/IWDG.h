/**
 * @file    IWDG.h
 * @brief   Independent watchdog (IWDG) driver.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The independent watchdog is clocked by the internal 40 kHz LSI oscillator and
 * therefore remains functional even if the main clock tree fails. It is used as
 * a last-resort recovery mechanism for the metronome firmware.
 *
 * @note
 * With a /64 prescaler and a reload value of 625, the timeout is
 * 625 / (40 000 / 64) = 1.0 s.
 *
 * @warning
 * The IWDG cannot be stopped once started. A firmware update or debug session
 * must either service the watchdog or hold the MCU in reset.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef IWDG_H
#define IWDG_H

#include "../CMSIS/Device/stm32f103xb.h"
#include "../Inc/timeout_handler.h"

/**
 * @def     IWDG_LSI_FREQ_HZ
 * @brief   Nominal LSI oscillator frequency in hertz.
 *
 * @note
 * The LSI is specified over a wide tolerance (typically 30-60 kHz). The
 * resulting watchdog timeout therefore varies accordingly; the 1 s value is a
 * nominal design target, not a precision timing guarantee.
 */
#define IWDG_LSI_FREQ_HZ 40000U

/* IWDG_KR register keys. */

/**
 * @def     IWDG_START_KEY
 * @brief   Key that starts the watchdog (0xCCCC).
 */
#define IWDG_START_KEY 0xCCCCU

/**
 * @def     IWDG_WRITE_KEY
 * @brief   Key that enables write access to PR and RLR (0x5555).
 */
#define IWDG_WRITE_KEY 0x5555U

/**
 * @def     IWDG_RELOAD_KEY
 * @brief   Key that reloads the watchdog counter (0xAAAA).
 */
#define IWDG_RELOAD_KEY 0xAAAAU

/* IWDG_PR register. */

/**
 * @def     IWDG_PR_DIVIDER
 * @brief   Prescaler divider value selected by @ref IWDG_PR_DIV4.
 */
#define IWDG_PR_DIVIDER 64U

/**
 * @def     IWDG_PR_DIV4
 * @brief   IWDG prescaler register code for LSI / 64.
 *
 * @details
 * The PR register stores a 3-bit code, not the divider itself. Code 4 selects
 * divider 64 according to the STM32F1 reference manual.
 */
#define IWDG_PR_DIV4 4U

/* IWDG_RLR register. */

/**
 * @def     IWDG_RL_VALUE
 * @brief   Reload value producing a nominal 1 s timeout.
 */
#define IWDG_RL_VALUE 625U

/**
 * @def     IWDG_EXPECTED_READY_VAL
 * @brief   Expected value of the PVU/RVU flags when the update is complete.
 */
#define IWDG_EXPECTED_READY_VAL 0U

/**
 * @def     IWDG_TIMEOUT_MS
 * @brief   Maximum time to wait for the prescaler/reload update, in ms.
 */
#define IWDG_TIMEOUT_MS 10U

/**
 * @brief   Start the independent watchdog.
 *
 * @note
 * The sequence is: enable write access, program the prescaler code and reload
 * value, wait for the PVU/RVU flags to clear, reload the counter and start the
 * watchdog.
 *
 * @warning
 * Must be called after @ref systick_init() because the register-update wait uses
 * @ref system_wait_for_flag(). Starting the IWDG before the time base exists
 * would make the timeout unobservable.
 */
void IWDG_Init(void);

/**
 * @brief   Reload the independent watchdog counter.
 *
 * @note
 * Writes the reload key (0xAAAA) to @c IWDG->KR. Must be called at least once
 * per watchdog period (nominally 1 s).
 *
 * @warning
 * Calling this function from a low-priority path while a higher-priority task is
 * stuck defeats the purpose of the watchdog. In this project the call is made
 * from the main loop.
 */
void IWDG_Restart(void);

#endif /* IWDG_H */
