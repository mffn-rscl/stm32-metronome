#include "../Inc/pwm_buzzer_driver.h"

void tim3_pwm_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->CRL &= ~(0xFU << 24);                     
    GPIOA->CRL |=  (GPIO_PA9_CONFIGURATION << 24);

    TIM3->PSC = TIM3_PSC_DIVIDER - 1;                         
    TIM3->ARR = (TIM3_TICK_PER_SEC / AUDIO_FREQ_HZ) - 1;              
  


    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);    
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;                          

    TIM3->CCR1 = ((TIM3->ARR + 1) * AUDIO_DUTY_PERCENT) / 100U; 
    TIM3->CR1 |= TIM_CR1_ARPE;                                   
    TIM3->EGR |= TIM_EGR_UG;                                 
    TIM3->CCER &= ~TIM_CCER_CC1E;

    TIM3->CR1 |= TIM_CR1_CEN;
}
