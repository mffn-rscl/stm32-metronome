#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"


void dma_channel6_enable(void);
void dma_channel6_disable(void);
void dma_channel6_init(void);
void dma_channel6_set_pma(uint8_t *framebuffer, uint16_t size);
#endif /*DMA_DRIVER_H*/
