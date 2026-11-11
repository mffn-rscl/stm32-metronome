/**
 * @file    timeout_handler.h
 * @brief   Deterministic SysTick time base and register-poll timeout helpers.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * This module provides a 1 ms monotonic software time base derived from the
 * Cortex-M3 SysTick timer and a bounded register-polling primitive used by all
 * blocking peripheral drivers. The design replaces unbounded
 * @c while(!(reg & flag)) loops with a deterministic timeout, following the
 * AUTOSAR-style "wait with deadline" pattern.
 *
 * @note
 * All timeout values are expressed in milliseconds and are measured with
 * unsigned 32-bit tick subtraction. The subtraction is wrap-safe for intervals
 * shorter than 2^32 ms (approximately 49.7 days).
 *
 * @warning
 * @ref system_wait_for_flag() is a busy-wait primitive. It must not be called
 * from a higher-priority interrupt while the same peripheral is being serviced
 * by lower-priority code (the drivers in this project are not re-entrant).
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef TIMEOUT_HANDLER_H
#define TIMEOUT_HANDLER_H

#include <stdint.h>

/*
 * The device header must be included before core_cm3.h so that IRQn_Type and
 * the device-specific interrupt numbers are defined. Including core_cm3.h
 * directly from this header breaks compilation when the header is included
 * first by a translation unit (for example timeout_handler.c).
 */
#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @def     SYSTICK_FREQ_DIVIDER
 * @brief   Number of SysTick clocks per software tick.
 *
 * @details
 * With a 72 MHz core clock, a divider of 1000 produces a 1 ms tick
 * (72 000 000 / 1000 = 72 000 clocks per tick).
 */
#define SYSTICK_FREQ_DIVIDER 1000U

/**
 * @enum    Hw_Status_t
 * @brief   Generic hardware operation status.
 *
 * @details
 * The enumeration is intentionally limited to two values so that it can be
 * returned directly from register-polling helpers and propagated to
 * @ref timeout_sleep().
 */
typedef enum
{
  SYSTEM_STATUS_TIMEOUT = 0U, /**< The operation did not complete before the deadline. */
  SYSTEM_STATUS_OK      = 1U  /**< The operation completed successfully.           */
} Hw_Status_t;

/**
 * @brief   Free-running millisecond counter incremented by @ref SysTick_handler().
 *
 * @note
 * Declared @c volatile because it is modified in interrupt context and read in
 * foreground context. Access is atomic on Cortex-M3 for aligned 32-bit words.
 */
extern volatile uint32_t g_sys_ticks;

/**
 * @brief   Configure SysTick to generate a 1 ms periodic interrupt.
 *
 * @param[in] cpu_freq_hz Core clock frequency in hertz (for example 72 000 000).
 *
 * @note
 * The SysTick clock source is the processor clock (AHB), not the external
 * reference clock. The function must be called after the PLL has been selected
 * as the system clock and before any driver that uses
 * @ref system_wait_for_flag().
 *
 * @warning
 * Calling this function more than once restarts the time base and may cause a
 * pending timeout to expire early.
 */
void systick_init(uint32_t cpu_freq_hz);

/**
 * @brief   Read the current millisecond tick count.
 *
 * @return  Current value of @ref g_sys_ticks in milliseconds.
 *
 * @note
 * The read is a single aligned 32-bit load and is therefore atomic on the
 * Cortex-M3. The value wraps naturally at 2^32 ms.
 */
uint32_t systick_get_tick(void);

/**
 * @brief   Poll a memory-mapped register until a flag pattern is observed or a
 *          millisecond deadline expires.
 *
 * @param[in] reg            Pointer to the volatile 32-bit register to poll.
 * @param[in] flag           Bit mask of interest.
 * @param[in] expected_value Required value of the masked bits. Use 0 to wait
 *                           for the bits to clear, or @p flag to wait for all
 *                           selected bits to be set.
 * @param[in] timeout_ms     Maximum time to wait in milliseconds.
 *
 * @retval  SYSTEM_STATUS_OK      The masked register value matched
 *                                @p expected_value before the deadline.
 * @retval  SYSTEM_STATUS_TIMEOUT The deadline expired first.
 *
 * @note
 * This function is non-blocking with respect to other interrupts: SysTick keeps
 * incrementing while the caller spins, so higher-priority interrupts remain
 * responsive. It does, however, block the calling context for up to
 * @p timeout_ms.
 *
 * @note
 * The elapsed-time calculation uses unsigned subtraction, which is correct
 * across the 32-bit tick wrap.
 *
 * @warning
 * @p reg must point to a valid 32-bit volatile register. Passing a null pointer
 * or a non-aligned address results in undefined behaviour (MISRA-C:2012
 * Rule 11.4 / Rule 11.6 deviation; see the project compliance notes).
 */
Hw_Status_t system_wait_for_flag(volatile uint32_t *reg,
                                 uint32_t flag,
                                 uint32_t expected_value,
                                 uint32_t timeout_ms);

/**
 * @brief   Convert a timeout status into a system-level fault reaction.
 *
 * @param[in] status Status returned by @ref system_wait_for_flag().
 *
 * @note
 * If @p status is @ref SYSTEM_STATUS_TIMEOUT the function performs a full
 * microcontroller reset through @c NVIC_SystemReset. A successful status is
 * ignored.
 *
 * @warning
 * This function has the intentional side effect of resetting the MCU. It never
 * returns when the status is @ref SYSTEM_STATUS_TIMEOUT.
 */
void timeout_sleep(Hw_Status_t status);

/**
 * @brief   SysTick exception handler.
 *
 * @note
 * ISR context. Increments @ref g_sys_ticks once per millisecond. The symbol
 * name must match the entry in @c Startup/vector_table.S exactly.
 *
 * @warning
 * Do not add peripheral access or long computations to this handler; it defines
 * the resolution of every timeout in the system.
 */
void SysTick_handler(void);

#endif /* TIMEOUT_HANDLER_H */
