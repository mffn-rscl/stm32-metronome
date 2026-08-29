#include "../Inc/dma_driver.h"
#include "../Inc/rcc_conf.h"


 

void dma_channel6_init(void)
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
  DMA1_Channel6->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
  
  /*INTERRUPTS ENABLE*/
  DMA1_Channel6->CCR |= DMA_CCR_TEIE | DMA_CCR_HTIE | DMA_CCR_TCIE;


  DMA1_Channel6->PA = (uint32_t)&I2C1->DR;
  DMA1_Channel6->MA = frame_buffer;
  
  DMA1_Channel6->CCR |= DMA_CCR_EN;
  NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}
