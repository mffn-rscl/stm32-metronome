/**
 * @file    timeout_handler.c
 * @brief   Implementation of the SysTick time base and bounded register polling.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The implementation deliberately avoids dynamic memory, floating point and
 * library calls. Only CMSIS core intrinsics are used, keeping the module
 * suitable for bare-metal, safety-oriented firmware.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/timeout_handler.h"

/**
 * @brief   Free-running millisecond counter.
 *
 * @note
 * Initialised to zero at startup and incremented once per SysTick interrupt.
 * The variable is shared between interrupt and foreground contexts and must
 * remain @c volatile.
 */
volatile uint32_t g_sys_ticks = 0U;

/**
 * @brief   Configure SysTick for a 1 ms periodic interrupt.
 *
 * @note
 * @c LOAD is written with the number of clocks per tick minus one. @c VAL is
 * cleared to force an immediate reload of the counter. The processor clock is
 * selected as the SysTick source.
 *
 * @note
 * The function is safe to call before global interrupts are enabled; the first
 * interrupt is taken only after the PRIMASK state allows it.
 */
void systick_init(uint32_t cpu_freq_hz)
{
  SysTick->LOAD = (cpu_freq_hz / SYSTICK_FREQ_DIVIDER) - 1U;
  SysTick->VAL  = 0U;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_TICKINT_Msk   |
                  SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief   Return the current millisecond tick count.
 *
 * @note
 * A single 32-bit read is atomic on Cortex-M3; no critical section is required.
 */
uint32_t systick_get_tick(void)
{
  return g_sys_ticks;
}

/**
 * @brief   SysTick exception handler.
 *
 * @note
 * ISR context. The handler performs only one increment so that the interrupt
 * latency remains deterministic. No peripheral access or function call is made.
 *
 * @warning
 * The symbol name must match the entry in @c Startup/vector_table.S exactly.
 * A mismatch silently binds the vector to the default infinite-loop handler.
 */
void SysTick_handler(void)
{
  g_sys_ticks++;
}

/**
 * @brief   Wait for a masked register value with a bounded timeout.
 *
 * @note
 * The loop condition and the elapsed-time test are evaluated on every
 * iteration. Because @ref g_sys_ticks is incremented by SysTick, the wait is
 * bounded even if the peripheral never sets the requested flag.
 *
 * @note
 * Unsigned subtraction of the start tick implements wrap-safe interval
 * measurement without a 64-bit counter.
 */
Hw_Status_t system_wait_for_flag(volatile uint32_t *reg,
                                 uint32_t flag,
                                 uint32_t expected_value,
                                 uint32_t timeout_ms)
{
  uint32_t start_tick = systick_get_tick();

  while ((*reg & flag) != expected_value)
  {
    uint32_t elapsed_time = systick_get_tick() - start_tick;

    if (elapsed_time >= timeout_ms)
    {
      return SYSTEM_STATUS_TIMEOUT;
    }
  }

  return SYSTEM_STATUS_OK;
}

/**
 * @brief   Reset the MCU when a hardware timeout has occurred.
 *
 * @note
 * A successful status returns immediately. A timeout triggers a full system
 * reset so that the application restarts from a known state instead of
 * continuing with an unresponsive peripheral.
 *
 * @warning
 * Intentional side effect: @c NVIC_SystemReset does not return. The
 * watchdog is not used here because the reset must be immediate and
 * deterministic.
 */
void timeout_sleep(Hw_Status_t status)
{
  if (status == SYSTEM_STATUS_TIMEOUT)
  {
    NVIC_SystemReset();
  }
}
