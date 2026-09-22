#include "../Inc/main.h"




int main(void)
{
  clock_init72mhz();
  i2c1_init();
  ssd1306_init();
  dma_channel6_init();

  tim2_init();
  button_regulation_init();
  tim3_pwm_init();
  /*DEBUG*/

  draw_bpm_prefix();
  ssd1306_update_display();
  /*DEBUG*/ 
  while(1)
  {
      
  }
}
