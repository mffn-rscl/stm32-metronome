#include "../Inc/tim2_bpm_clock_control.h"

volatile static uint8_t current_bpm = 60; 

void tim2_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = TIM2_PSC_DIVIDER - 1;

    TIM2->ARR = ((TIM2_TICK_PER_SEC + (current_bpm / 2)) / current_bpm) - 1;

    TIM2->EGR |= TIM_EGR_UG;

    TIM2->CR1 |= TIM_CR1_ARPE;

    TIM2->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}


void button_regulation_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
  
  GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_CNF8);
  /*enabling the buttons*/
  GPIOA->CRH |= GPIOA_CRH_BUTTON_CONFIGURATION;
  GPIOA->ODR |= EXTI_PA8_PIN | EXTI_PA9_PIN;

  AFIO->EXTICR[2] &= ~(0xFFU);
  
  EXTI->IMR |= EXTI_PA8_PIN | EXTI_PA9_PIN;
  EXTI->FTSR |= EXTI_PA8_PIN | EXTI_PA9_PIN;
  EXTI->RTSR &= ~(EXTI_PA8_PIN | EXTI_PA9_PIN);
  EXTI->PR =  EXTI_PA8_PIN | EXTI_PA9_PIN;
  
  NVIC_SetPriority(EXTI9_5_IRQn, 2);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI9_5_IRQ_handler(void)
{
  if(EXTI->PR & EXTI_PA8_PIN)
  {
     EXTI->PR = EXTI_PA8_PIN;
     if(current_bpm + 1 <= MAX_BPM_VALUE)
     {
      current_bpm++;
     }

  }
  else if(EXTI->PR & EXTI_PA9_PIN)
  {
    EXTI->PR = EXTI_PA9_PIN;
     if(current_bpm - 1 >= MIN_BPM_VALUE)
     {
      current_bpm--;
     }

  }
  TIM2->ARR = ((TIM2_TICK_PER_SEC + (current_bpm / 2)) / current_bpm) - 1;
}


void TIM2_IRQ_handler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        //pwm start/ends here
    }
}
