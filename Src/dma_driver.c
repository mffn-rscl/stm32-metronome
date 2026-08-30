#include "../Inc/dma_driver.h"
#include "../Inc/i2c_driver.h"
void dma_channel6_enable(void)
{
  DMA1_Channel6->CCR |= DMA_CCR_EN;
}

void dma_channel6_disable(void)
{
  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
}
 
void dma_channel6_set_pma(uint8_t *framebuffer, uint16_t size)
{
  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
  DMA1_Channel6->CMAR = (uint32_t)framebuffer;
  DMA1_Channel6->CNDTR = size;

}

void dma_channel6_init(void)
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  DMA1_Channel6->CCR &= ~DMA_CCR_EN;
  DMA1_Channel6->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;
  
  DMA1_Channel6->CPAR = (uint32_t)&I2C1->DR;
  /*INTERRUPTS ENABLE*/
  DMA1_Channel6->CCR |= DMA_CCR_TEIE | DMA_CCR_TCIE;
  
  NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}



void DMA1_chan6_IRQ_handler(void)
{
  if(DMA1->ISR & DMA_ISR_TCIF6)
  {
    DMA1->IFCR = DMA_IFCR_CTCIF6; 
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    i2c1_stop_condition();
  }
  
  if(DMA1->ISR & DMA_ISR_TEIF6)
  {
    DMA1->IFCR = DMA_IFCR_CTEIF6; 
    DMA1_Channel6->CCR &= ~DMA_CCR_EN;
    i2c1_stop_condition();
  }
}
