#include "../Inc/IWDG.h"




void IWDG_Init(void)
{
  IWDG->KR = IWDG_WRITE_KEY;
  
  IWDG->PR = IWDG_LSI_FREQ_HZ / IWDG_PR_DIVIDER;
  
  IWDG->RLR = IWDG_RL_VALUE;

  IWDG->KR = IWDG_RELOAD_KEY; 

  IWDG->KR = IWDG_START_KEY; 
}
void IWDG_Restart(void)
{

  IWDG->KR = IWDG_START_KEY; 
}
