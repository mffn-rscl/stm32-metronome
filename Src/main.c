#include "../Inc/main.h"


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
