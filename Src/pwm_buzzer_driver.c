/**
 * @file    pwm_buzzer_driver.c
 * @brief   TIM3 PWM audio-click generator implementation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The timer is configured for edge-aligned PWM with a 1 MHz counter and a
 * 1 kHz period. The compare value sets a 50 % duty cycle. The output pin is
 * enabled and disabled by the TIM2 beat state machine, which produces a short
 * burst at every beat instead of a continuous tone.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/pwm_buzzer_driver.h"

/**
 * @brief   Initialise TIM3 Channel 1 on PA6.
 *
 * @note
 * PWM mode 1 is selected by setting both @c OC1M bits. The compare preload bit
 * ensures that a new compare value is latched at the next update event.
 *
 * @note
 * The GPIO nibble is written to bits 24-27 of @c GPIOA->CRL, which control
 * PA6. The previous macro name referred to PA9 but the bit position was always
 * PA6; the name has been corrected during documentation cleanup.
 */
void tim3_pwm_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  /* PA6: alternate-function push-pull, 50 MHz. */
  GPIOA->CRL &= ~(0xFU << 24);
  GPIOA->CRL |= (GPIO_PA6_AF_PP_CONFIGURATION << 24);

  TIM3->PSC = TIM3_PSC_DIVIDER - 1U;
  TIM3->ARR = (TIM3_TICK_PER_SEC / AUDIO_FREQ_HZ) - 1U;

  /* PWM mode 1 with compare preload. */
  TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM3->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
  TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

  TIM3->CCR1 = ((TIM3->ARR + 1U) * AUDIO_DUTY_PERCENT) / 100U;
  TIM3->CR1 |= TIM_CR1_ARPE;
  TIM3->EGR |= TIM_EGR_UG;

  /* Start with the output disabled; TIM2 gates each click. */
  TIM3->CCER &= ~TIM_CCER_CC1E;

  TIM3->CR1 |= TIM_CR1_CEN;
}
