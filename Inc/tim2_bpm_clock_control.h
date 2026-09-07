#ifndef TIM2_BPM_CLOCK_CONTROL 
#define TIM2_BPM_CLOCK_CONTROL

#define TIM2_PSC_DIVIDER 7200U
#define TIM2_TICK_PER_SEC 600000U

void tim2_init(uint16_t initial_bpm);

#endif /*TIM2_BPM_CLOCK_CONTROL*/ 
