/**
 * @file    tim2_bpm_clock_control.h
 * @brief   Tempo generation and push-button control for the metronome.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * TIM2 generates the musical beat. With a 10 kHz counter tick, the auto-reload
 * value is calculated from the requested beats per minute. A compare channel
 * (CC2) defines the audible click length. The same module configures the two
 * BPM push-buttons on PA8/PA9 as falling-edge EXTI inputs with internal
 * pull-ups.
 *
 * @note
 * The button inputs are additionally filtered by 0.1 uF capacitors on the
 * board. The firmware performs no software debouncing; the RC network is
 * responsible for contact bounce suppression.
 *
 * @warning
 * @ref current_bpm is shared between the EXTI interrupt and the display path.
 * It is declared @c volatile and is only 8 bits wide, so Cortex-M3 byte accesses
 * are atomic. Do not replace it with a multi-byte type without adding a
 * critical section.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef TIM2_BPM_CLOCK_CONTROL
#define TIM2_BPM_CLOCK_CONTROL

#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @def     TIM2_PSC_DIVIDER
 * @brief   TIM2 prescaler divider: 72 MHz / 7200 = 10 kHz counter clock.
 */
#define TIM2_PSC_DIVIDER 7200U

/**
 * @def     TIM2_TICK_HZ
 * @brief   TIM2 counter frequency in hertz after the prescaler.
 */
#define TIM2_TICK_HZ 10000U

/**
 * @def     TIM2_TICKS_PER_MINUTE
 * @brief   Number of TIM2 ticks in one minute (10 kHz x 60 s).
 *
 * @details
 * The auto-reload value for a given BPM is
 * @c TIM2_TICKS_PER_MINUTE / bpm - 1.
 */
#define TIM2_TICKS_PER_MINUTE 600000U

/**
 * @def     AFIO_EXTI_BUTTONS_MASK
 * @brief   AFIO EXTICR mask selecting port A for EXTI lines 8 and 9.
 *
 * @note
 * A value of 0x00 maps EXTI8/EXTI9 to PA8/PA9. The mask is applied to
 * @c AFIO->EXTICR[2].
 */
#define AFIO_EXTI_BUTTONS_MASK 0x00U

/**
 * @def     GPIOA_CRH_BUTTON_CONFIGURATION
 * @brief   GPIOA CRH nibbles for PA8/PA9: input with pull-up/pull-down.
 *
 * @details
 * 0x88 = 1000 1000: MODE = 00 (input), CNF = 10 (input with pull-up/down).
 */
#define GPIOA_CRH_BUTTON_CONFIGURATION 0x88U

/**
 * @def     EXTI_PA8_PIN
 * @brief   EXTI line 8 bit mask (BPM increment button, SW2).
 */
#define EXTI_PA8_PIN (0x1U << 8)

/**
 * @def     EXTI_PA9_PIN
 * @brief   EXTI line 9 bit mask (BPM decrement button, SW1).
 */
#define EXTI_PA9_PIN (0x1U << 9)

/**
 * @def     BUZZER_CLICK_DURATION_MS
 * @brief   Duration of the audible click in milliseconds.
 *
 * @note
 * The value is converted to TIM2 ticks by
 * @ref BUZZER_CLICK_DURATION_TICKS. At the current 10 kHz tick rate, 2 ms
 * corresponds to 20 timer counts.
 */
#define BUZZER_CLICK_DURATION_MS 2U

/**
 * @def     BUZZER_CLICK_DURATION_TICKS
 * @brief   Click duration expressed in TIM2 counter ticks.
 */
#define BUZZER_CLICK_DURATION_TICKS \
  (BUZZER_CLICK_DURATION_MS * (TIM2_TICK_HZ / 1000U))

/**
 * @def     MAX_BPM_VALUE
 * @brief   Upper tempo limit in beats per minute.
 */
#define MAX_BPM_VALUE 250U

/**
 * @def     MIN_BPM_VALUE
 * @brief   Lower tempo limit in beats per minute.
 */
#define MIN_BPM_VALUE 30U

/**
 * @brief   Current tempo in beats per minute.
 *
 * @note
 * Written by @ref EXTI9_5_IRQ_handler() and read by @ref tim2_init() and the
 * display update path. The initial value is 60 BPM.
 */
extern volatile uint8_t current_bpm;

/**
 * @brief   Initialise TIM2 for beat generation.
 *
 * @note
 * Configures a 10 kHz counter, calculates the auto-reload value for
 * @ref current_bpm, programs compare channel 2 for the click length, enables
 * the update and CC2 interrupts and starts the timer.
 *
 * @note
 * TIM2 interrupt priority is 0 (highest), so the beat edge is never delayed by
 * display or button processing.
 *
 * @warning
 * Must be called after @ref clock_init72mhz().
 */
void tim2_init(void);

/**
 * @brief   Configure PA8/PA9 as debounced BPM push-button inputs.
 *
 * @note
 * Enables GPIOA and AFIO, selects input-with-pull-up mode, maps EXTI8/EXTI9 to
 * port A, enables falling-edge triggers and unmasks the EXTI9_5 interrupt.
 *
 * @note
 * PA8 is the increment button (SW2) and PA9 is the decrement button (SW1) on
 * the Rev v1.0 board. The start/stop button on PA10 is not configured here.
 *
 * @warning
 * The external 0.1 uF capacitors must be fitted for reliable operation. Without
 * them a single press may generate multiple interrupts.
 */
void button_regulation_init(void);

/**
 * @brief   TIM2 interrupt handler.
 *
 * @note
 * ISR context. The update event starts the audio PWM burst by setting
 * @c TIM3->CCER.CC1E. The CC2 event stops the burst by clearing the same bit.
 * The handler performs no blocking operations.
 *
 * @warning
 * The handler assumes that TIM3 has already been initialised by
 * @ref tim3_pwm_init().
 */
void TIM2_IRQ_handler(void);

/**
 * @brief   EXTI line 9..5 interrupt handler for the BPM buttons.
 *
 * @note
 * ISR context. Pending EXTI flags are cleared by writing 1 to @c EXTI->PR. The
 * tempo is clamped to [@ref MIN_BPM_VALUE, @ref MAX_BPM_VALUE], the display is
 * updated and the TIM2 auto-reload value is recalculated.
 *
 * @warning
 * This handler calls @ref ssd1306_update_display(), which starts a blocking I2C
 * transaction before handing the frame buffer to DMA. Keep the I2C timeout
 * short and avoid adding long operations to this ISR.
 */
void EXTI9_5_IRQ_handler(void);

#endif /* TIM2_BPM_CLOCK_CONTROL */
