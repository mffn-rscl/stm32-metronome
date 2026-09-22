#ifndef PWM_BUZZER_DRIVER_H
#define PWM_BUZZER_DRIVER_H

#include "../CMSIS/Device/stm32f103xb.h"
#define TIM3_PSC_DIVIDER       72U     
#define AUDIO_FREQ_HZ          4000U    
#define AUDIO_DUTY_PERCENT     50U 
#define GPIO_PA9_CONFIGURATION 0xBU 
#define TIM3_TICK_PER_SEC      1000000U

void tim3_pwm_init(void);

#endif /*PWM_BUZZER_DRIVER_H*/
