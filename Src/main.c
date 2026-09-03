#include "../Inc/main.h"


volatile uint8_t ssd1306_frame_buffer[FRAME_BUFFER_SIZE];


int main(void)
{
  clock_init72mhz();
  
  i2c1_init();
  
  ssd1306_init();

  dma_channel6_init();
  
  while(1)
  {
    
  }
}
