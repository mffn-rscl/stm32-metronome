/**
 * @file    rcc_conf.c
 * @brief   Register-level 72 MHz clock-tree initialisation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * Implements the STM32F103 clock start-up sequence:
 * 1. Enable the 8 MHz HSE and wait for HSERDY.
 * 2. Enable the flash prefetch buffer and program 2 wait states.
 * 3. Set AHB /1, APB2 /1 and APB1 /2.
 * 4. Select HSE as PLL source and configure the PLL for x9.
 * 5. Enable the PLL and wait for PLLRDY.
 * 6. Switch SYSCLK to the PLL and wait for SWS.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/rcc_conf.h"

/**
 * @brief   Configure the clock tree for 72 MHz SYSCLK.
 *
 * @note
 * All read-modify-write sequences preserve unrelated RCC bits. The function is
 * intended to be called exactly once from @ref main() before any peripheral is
 * initialised.
 *
 * @warning
 * No timeout is applied to the oscillator or PLL ready loops. See
 * @ref clock_init72mhz() in @c rcc_conf.h for the rationale.
 */
void clock_init72mhz(void)
{
  /* 1. Start the 8 MHz HSE crystal and wait for it to stabilise. */
  RCC->CR |= RCC_CR_HSEON;
  while ((RCC->CR & RCC_CR_HSERDY) == 0U)
  {
    /* Intentionally empty: bounded only by crystal start-up time. */
  }

  /* 2. Flash: enable prefetch and select two wait states for 72 MHz. */
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_2;

  /* 3. AHB /1, APB2 /1, APB1 /2 (36 MHz maximum). */
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1);
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

  /* 4. PLL source = HSE, multiplier = x9. */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
  RCC->CFGR |= (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

  /* 5. Enable the PLL and wait for lock. */
  RCC->CR |= RCC_CR_PLLON;
  while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
  {
    /* Intentionally empty: PLL lock time is specified by the datasheet. */
  }

  /* 6. Select the PLL as SYSCLK and wait for the switch to complete. */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
  {
    /* Intentionally empty: switch is hardware-sequenced. */
  }
}
