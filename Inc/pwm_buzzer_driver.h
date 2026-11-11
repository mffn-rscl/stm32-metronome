/**
 * @file    pwm_buzzer_driver.h
 * @brief   TIM3 PWM audio-click generator for the LM386 amplifier.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * TIM3 Channel 1 produces a 1 kHz square wave on PA6. The output is normally
 * disabled; the TIM2 beat interrupt enables it for the click duration and the
 * TIM2 compare interrupt disables it again. The signal is fed through a 10 kOhm
 * series resistor to the LM386 non-inverting input.
 *
 * @note
 * A 1 kHz square wave is used as the click timbre. The LM386 and the 8 ohm
 * speaker provide the acoustic output; no DAC or audio sample stream is
 * required.
 *
 * @warning
 * PA6 is also the TIM3_CH1 alternate-function pin. Do not configure it as a
 * GPIO output elsewhere in the application.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef PWM_BUZZER_DRIVER_H
#define PWM_BUZZER_DRIVER_H

#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @def     TIM3_PSC_DIVIDER
 * @brief   TIM3 prescaler divider: 72 MHz / 72 = 1 MHz counter clock.
 */
#define TIM3_PSC_DIVIDER 72U

/**
 * @def     AUDIO_FREQ_HZ
 * @brief   PWM click frequency in hertz.
 */
#define AUDIO_FREQ_HZ 1000U

/**
 * @def     AUDIO_DUTY_PERCENT
 * @brief   PWM duty cycle in percent (50 % square wave).
 */
#define AUDIO_DUTY_PERCENT 50U

/**
 * @def     GPIO_PA6_AF_PP_CONFIGURATION
 * @brief   GPIOA CRL nibble for PA6: alternate-function push-pull, 50 MHz.
 *
 * @details
 * 0xB = 1011: MODE = 11 (50 MHz output), CNF = 10 (alternate-function
 * push-pull). The nibble is shifted into bits 24-27 of @c GPIOA->CRL, which
 * correspond to pin 6.
 */
#define GPIO_PA6_AF_PP_CONFIGURATION 0xBU

/**
 * @def     TIM3_TICK_PER_SEC
 * @brief   TIM3 counter frequency in hertz after the prescaler.
 */
#define TIM3_TICK_PER_SEC 1000000U

/**
 * @brief   Initialise TIM3 Channel 1 for PWM click generation.
 *
 * @note
 * Configures PA6 as TIM3_CH1 alternate-function push-pull, sets a 1 kHz
 * auto-reload value and a 50 % compare value, enables auto-reload preload and
 * starts the counter.
 *
 * @note
 * The channel output is deliberately left disabled
 * (@c TIM3->CCER.CC1E = 0). The TIM2 interrupt handler gates the output for the
 * duration of each click.
 *
 * @warning
 * Must be called after @ref clock_init72mhz(). The function does not enable the
 * TIM3 interrupt because TIM3 is used in output-compare PWM mode only.
 */
void tim3_pwm_init(void);

#endif /* PWM_BUZZER_DRIVER_H */
