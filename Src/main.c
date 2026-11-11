/**
 * @file    main.c
 * @brief   Application entry point for the STM32F103 bare-metal metronome.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * Initialisation order:
 * 1. 72 MHz clock tree (@ref clock_init72mhz).
 * 2. 1 ms SysTick time base (@ref systick_init).
 * 3. I2C1 and SSD1306 display (@ref i2c1_init, @ref ssd1306_init).
 * 4. DMA1 Channel 6 for display uploads (@ref dma_channel6_init).
 * 5. TIM2 tempo generator and BPM buttons (@ref tim2_init,
 *    @ref button_regulation_init).
 * 6. TIM3 PWM click generator (@ref tim3_pwm_init).
 * 7. Initial display frame and independent watchdog (@ref IWDG_Init).
 *
 * The main loop only services the watchdog. Beat generation, button handling
 * and display transfers are interrupt-driven, so the loop remains available for
 * future low-priority work without affecting timing.
 *
 * @note
 * The firmware is bare-metal and uses CMSIS register definitions only. There is
 * no HAL, no RTOS and no heap.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/main.h"

/**
 * @brief   Application entry point.
 *
 * @retval  int Never returns; the function contains the infinite main loop.
 *
 * @note
 * The display is rendered once at boot so that the user sees the current tempo
 * before pressing any button. Subsequent updates are triggered by the BPM
 * button interrupt.
 *
 * @note
 * The watchdog is reloaded on every main-loop iteration. All long-running work
 * must therefore complete in less than the nominal 1 s IWDG period.
 *
 * @warning
 * Do not add blocking delays to the main loop without also servicing the
 * watchdog; otherwise the MCU will reset every second.
 */
int main(void)
{
  clock_init72mhz();
  systick_init(CPU_FREQ_HZ);
  i2c1_init();
  ssd1306_init();
  dma_channel6_init();

  tim2_init();
  button_regulation_init();
  tim3_pwm_init();

  /* Render the initial tempo screen. */
  draw_bpm_prefix();
  set_current_bpm(current_bpm);
  ssd1306_update_display();

  IWDG_Init();

  while (1)
  {
    IWDG_Restart();
  }
}
