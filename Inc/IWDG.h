#ifndef IWDG_H
#define IWDG_H

#include "../CMSIS/Device/stm32f103xb.h"

#define IWDG_LSI_FREQ_HZ  40000U


/*IWDG_KR REGISTER*/

#define IWDG_START_KEY    0xCCCCU
#define IWDG_WRITE_KEY    0x5555U
#define IWDG_RELOAD_KEY   0xAAAAU

/*IWDG_PR REGISTER*/
#define IWDG_LSI_FREQ_HZ  40000U
#define IWDG_PR_DIVIDER   64U

/*IWDG_RLR REGISTER*/
#define IWDG_RL_VALUE     625U

void IWDG_Init(void);
void IWDG_Restart(void);

#endif /*IWDG_H*/
