#ifndef TIM2_BPM_CLOCK_CONTROL 
#define TIM2_BPM_CLOCK_CONTROL

#define TIM2_PSC_DIVIDER 7200U
#define TIM2_TICK_PER_SEC 600000U

#define AFIO_EXTI_BUTTONS_MASK 0x00U
#define GPIOA_CRH_BUTTON_CONFIGURATION (0x88U << 8)
#define EXTI_PA8_PIN   (0x1U << 8)
#define EXTI_PA9_PIN   (0x1U << 9)

#define MAX_BPM_VALUE 250U
#define MIN_BPM_VALUE 30U

void tim2_init(void);


void button_regulation_init(void);



#endif /*TIM2_BPM_CLOCK_CONTROL*/ 
