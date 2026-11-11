/**
 * @file    tim2_bpm_clock_control.c
 * @brief   TIM2 tempo generator and BPM button interrupt implementation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * TIM2 is clocked from APB1 (36 MHz) with a prescaler of 7200, producing a
 * 10 kHz counter. The auto-reload register is updated whenever the tempo
 * changes. Compare channel 2 is used as a one-shot click-length timer: the
 * update event enables the TIM3 PWM output and the CC2 event disables it.
 *
 * @note
 * The implementation keeps all ISR work short except for the display update in
 * the EXTI handler. That update is a deliberate trade-off: it keeps the user
 * interface responsive without a main-loop scheduler.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/tim2_bpm_clock_control.h"
#include "../Inc/ssd1306.h"

/**
 * @brief   Current tempo in beats per minute.
 *
 * @note
 * Initialised to 60 BPM. Modified only by @ref EXTI9_5_IRQ_handler().
 */
volatile uint8_t current_bpm = 60U;

/**
 * @brief   Initialise TIM2 for tempo generation.
 *
 * @note
 * The auto-reload value is rounded to the nearest count by adding
 * @c bpm / 2 before the integer division. This reduces the tempo error at high
 * BPM values.
 *
 * @note
 * @c TIM2->EGR is written to force an update event so that the prescaler and
 * auto-reload values take effect immediately.
 */
void tim2_init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  TIM2->PSC = TIM2_PSC_DIVIDER - 1U;

  TIM2->ARR = ((TIM2_TICKS_PER_MINUTE + (current_bpm / 2U)) / current_bpm) - 1U;

  TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
  TIM2->CCR2 = BUZZER_CLICK_DURATION_TICKS;

  TIM2->EGR |= TIM_EGR_UG;

  TIM2->CR1 |= TIM_CR1_ARPE;

  /* Enable update and compare-channel-2 interrupts. */
  TIM2->DIER |= TIM_DIER_UIE | TIM_DIER_CC2IE;

  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_EnableIRQ(TIM2_IRQn);

  TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief   Configure the BPM push-buttons on PA8 and PA9.
 *
 * @note
 * Both pins are inputs with internal pull-ups and generate a falling-edge
 * interrupt when pressed. The external 0.1 uF capacitors provide hardware
 * debouncing.
 *
 * @note
 * EXTI9_5 covers lines 5 through 9. Lines 5, 6 and 7 are left masked.
 */
void button_regulation_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  GPIOA->CRH &= ~(0xFFU);

  /* PA8/PA9: input with pull-up/pull-down. */
  GPIOA->CRH |= GPIOA_CRH_BUTTON_CONFIGURATION;
  GPIOA->ODR |= EXTI_PA8_PIN | EXTI_PA9_PIN;

  /* Map EXTI8/EXTI9 to port A. */
  AFIO->EXTICR[2] &= ~(0xFFU);

  EXTI->IMR  |= EXTI_PA8_PIN | EXTI_PA9_PIN;
  EXTI->FTSR |= EXTI_PA8_PIN | EXTI_PA9_PIN;
  EXTI->RTSR &= ~(EXTI_PA8_PIN | EXTI_PA9_PIN);
  EXTI->PR    = EXTI_PA8_PIN | EXTI_PA9_PIN;

  NVIC_SetPriority(EXTI9_5_IRQn, 2);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief   Handle BPM increment/decrement button presses.
 *
 * @note
 * ISR context. A pending flag is cleared by writing 1 to the corresponding
 * @c EXTI->PR bit. The tempo is clamped before the display and timer are
 * updated, so no wrap-around can occur at the limits.
 *
 * @note
 * The display update is asynchronous with respect to the I2C transfer: the
 * function returns after starting DMA.
 *
 * @warning
 * Performs a blocking I2C window command before starting DMA. This is the
 * longest operation in the handler.
 */
void EXTI9_5_IRQ_handler(void)
{
  if ((EXTI->PR & EXTI_PA8_PIN) != 0U)
  {
    EXTI->PR = EXTI_PA8_PIN;

    if (current_bpm < MAX_BPM_VALUE)
    {
      current_bpm++;
    }
  }

  if ((EXTI->PR & EXTI_PA9_PIN) != 0U)
  {
    EXTI->PR = EXTI_PA9_PIN;

    if (current_bpm > MIN_BPM_VALUE)
    {
      current_bpm--;
    }
  }

  set_current_bpm(current_bpm);
  ssd1306_update_display();

  TIM2->ARR = ((TIM2_TICKS_PER_MINUTE + (current_bpm / 2U)) / current_bpm) - 1U;
}

/**
 * @brief   TIM2 update and compare interrupt handler.
 *
 * @note
 * ISR context. The update event marks the start of a beat and enables the TIM3
 * PWM output. The CC2 event marks the end of the click window and disables the
 * PWM output.
 *
 * @note
 * The interrupt flags are cleared by writing zero to the corresponding @c SR
 * bits. No read-modify-write of unrelated flags is performed.
 */
void TIM2_IRQ_handler(void)
{
  if ((TIM2->SR & TIM_SR_UIF) != 0U)
  {
    TIM2->SR &= ~TIM_SR_UIF;

    /* Start the PWM click. */
    TIM3->CCER |= TIM_CCER_CC1E;
  }

  if ((TIM2->SR & TIM_SR_CC2IF) != 0U)
  {
    TIM2->SR &= ~TIM_SR_CC2IF;

    /* Stop the PWM click. */
    TIM3->CCER &= ~TIM_CCER_CC1E;
  }
}
