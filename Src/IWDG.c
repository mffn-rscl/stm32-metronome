/**
 * @file    IWDG.c
 * @brief   Independent watchdog implementation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The watchdog is configured for a nominal 1 s timeout from the 40 kHz LSI.
 * Register updates are protected by the standard key sequence and bounded by a
 * 10 ms timeout.
 *
 * @note
 * The IWDG is intentionally started late in the initialisation sequence, after
 * all peripherals have been configured. This prevents a watchdog reset during
 * display or timer start-up.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/IWDG.h"

/**
 * @brief   Configure and start the independent watchdog.
 *
 * @note
 * @c IWDG->PR expects a 3-bit prescaler code. The previous implementation wrote
 * the divider value (625) directly, which selected an unintended prescaler; the
 * code now writes @ref IWDG_PR_DIV4 explicitly.
 *
 * @note
 * The PVU and RVU flags are polled with @ref system_wait_for_flag(). If the
 * update does not complete within @ref IWDG_TIMEOUT_MS, the MCU is reset by
 * @ref timeout_sleep().
 */
void IWDG_Init(void)
{
  IWDG->KR = IWDG_WRITE_KEY;

  IWDG->PR  = IWDG_PR_DIV4;
  IWDG->RLR = IWDG_RL_VALUE;

  /* Wait until the prescaler and reload registers are updated. */
  timeout_sleep(
      system_wait_for_flag(
          &IWDG->SR, (IWDG_SR_PVU | IWDG_SR_RVU),
          IWDG_EXPECTED_READY_VAL, IWDG_TIMEOUT_MS));

  IWDG->KR = IWDG_RELOAD_KEY;
  IWDG->KR = IWDG_START_KEY;
}

/**
 * @brief   Reload the watchdog counter.
 *
 * @note
 * The reload key is 0xAAAA. Writing the start key here (as in an earlier
 * revision) would not reload the counter and would cause an unintended reset.
 */
void IWDG_Restart(void)
{
  IWDG->KR = IWDG_RELOAD_KEY;
}
