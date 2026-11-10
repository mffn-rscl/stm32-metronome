#include "../Inc/IWDG.h"




void IWDG_Init(void)
{
  IWDG->KR = IWDG_WRITE_KEY;
  
  IWDG->PR = IWDG_LSI_FREQ_HZ / IWDG_PR_DIVIDER;
  
  IWDG->RLR = IWDG_RL_VALUE;
  /*wait until all registers shall be ready*/
  timeout_sleep(
    system_wait_for_flag(
      &IWDG->SR, (IWDG_SR_PVU | IWDG_SR_RVU), 
      IWDG_EXPECTED_READY_VAL, IWDG_TIMEOUT_MS
      )
  );
   
  // while (IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) {}
 
  IWDG->KR = IWDG_RELOAD_KEY; 

  IWDG->KR = IWDG_START_KEY; 
}
void IWDG_Restart(void)
{
  IWDG->KR = IWDG_START_KEY; 
}
