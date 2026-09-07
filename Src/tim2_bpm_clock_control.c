#include "../Inc/tim2_bpm_clock_control.h"


void tim2_init(uint16_t initial_bpm)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = TIM2_PSC_DIVIDER - 1;

    TIM2->ARR = ((TIM2_TICK_PER_SEC + (initial_bpm / 2)) / initial_bpm) - 1;

    TIM2->EGR |= TIM_EGR_UG;

    TIM2->CR1 |= TIM_CR1_ARPE;

    TIM2->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQ_handler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        //pwm start/ends here
    }
}
