#ifndef TIMEOUT_HANDLER_H
#define TIMEOUT_HANDLER_H

#include "../CMSIS/Include/core_cm3.h"
#define SYSTICK_FREQ_DIVIDER 1000U
typedef enum 
{
  SYSTEM_STATUS_TIMEOUT =  0U,
  SYSTEM_STATUS_OK = 1U
}Hw_Status_t;

extern volatile uint32_t g_sys_ticks;

static inline void systick_init(uint32_t cpu_freq_hz);

static inline uint32_t systick_get_tick(void);

static inline Hw_Status_t system_wait_for_flag(volatile uint32_t *reg, uint32_t flag,uint32_t expected_value, uint32_t timeout_ms);

static inline void timeout_sleep(Hw_Status_t status);
#endif /*TIMEOUT_HANDLER_H*/
