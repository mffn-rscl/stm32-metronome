#include "../Inc/timeout_handler.h"

volatile uint32_t g_sys_ticks = 0U;

static inline void systick_init(uint32_t cpu_freq_hz)
{
  SysTick->LOAD = (cpu_freq_hz / SYSTICK_FREQ_DIVIDER) - 1U;
  SysTick->VAL = 0U;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

static inline uint32_t systick_get_tick(void)
{
  return g_sys_ticks;
}

void SysTick_Handler(void)
{
    g_sys_ticks++;
}


static inline Hw_Status_t system_wait_for_flag(volatile uint32_t *reg, uint32_t flag,uint32_t expected_value, uint32_t timeout_ms)
{
  uint32_t start_tick = systick_get_tick();

  while((*reg & flag) != expected_value)
  {
    uint32_t elapsed_time = systick_get_tick() - start_tick;
    if(elapsed_time >= timeout_ms)
    {
      return SYSTEM_STATUS_TIMEOUT;
    }
  }

  return SYSTEM_STATUS_OK;
}


static inline void timeout_sleep(Hw_Status_t status)
{
  if(status == SYSTEM_STATUS_TIMEOUT)
  { 
    NVIC_SystemReset();
  }
}
