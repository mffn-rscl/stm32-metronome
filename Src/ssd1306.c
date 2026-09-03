#include "../Inc/ssd1306.h"
#include "../Inc/i2c_driver.h"
#include "../Inc/dma_driver.h"


void ssd1306_init(void)
{
    i2c1_start();
    i2c1_slave_adress(SSD1306_ADDR);

    i2c1_data(SSD1306_CONTROL_BYTE_COMMAND_STREAM);

    i2c1_data(SSD1306_CONFIGURATION_TURN_DISPLAY_OFF);

    i2c1_data(SSD1306_CONFIGURATION_MUX_RATIO_CMD);
    i2c1_data(SSD1306_CONFIGURATION_MUX_RATIO_ARG);

    i2c1_data(SSD1306_CONFIGURATION_DISPLAY_OFFSET_CMD);
    i2c1_data(SSD1306_CONFIGURATION_DISPLAY_OFFSET_ARG);

    i2c1_data(SSD1306_CONFIGURATION_MEM_ADDR_MODE_CMD);
    i2c1_data(SSD1306_CONFIGURATION_MEM_ADDR_MODE_HORIZ);

    i2c1_data(SSD1306_CONFIGURATION_SET_START_LINE);

    i2c1_data(SSD1306_CONFIGURATION_SEGMENT_REMAP);

    i2c1_data(SSD1306_CONFIGURATION_SCAN_DIRECTION);

    i2c1_data(SSD1306_CONFIGURATION_COM_PINS_CMD);
    i2c1_data(SSD1306_CONFIGURATION_COM_PINS_ARG);

    i2c1_data(SSD1306_CONFIGURATION_CONTRAST_CMD);
    i2c1_data(SSD1306_CONFIGURATION_CONTRAST_ARG);

    i2c1_data(SSD1306_CONFIGURATION_ENTIRE_DISPLAY_RAM);

    i2c1_data(SSD1306_CONFIGURATION_SET_NORMAL_DISPLAY);

    i2c1_data(SSD1306_CONFIGURATION_SET_OSC_FREQ_CMD);
    i2c1_data(SSD1306_CONFIGURATION_SET_OSC_FREQ_ARG);

    i2c1_data(SSD1306_CONFIGURATION_CHARGE_PUMP_CMD);
    i2c1_data(SSD1306_CONFIGURATION_CHARGE_PUMP_ARG);

    i2c1_data(SSD1306_CONFIGURATION_TURN_DISPLAY_ON);

    i2c1_stop_condition();
}

void ssd1306_set_resolution(uint8_t start_x, uint8_t end_x, uint8_t start_y, uint8_t end_y)
{
  i2c1_start();
  i2c1_slave_adress(SSD1306_ADDR);
  i2c1_data(SSD1306_CONTROL_BYTE_COMMAND_STREAM);

  i2c1_data(SSD1306_CONFIGURATION_SET_COL_ADDR);
  i2c1_data(start_x);
  i2c1_data(end_x);
  
  i2c1_data(SSD1306_CONFIGURATION_SET_ROW_ADDR);
  i2c1_data(start_y);
  i2c1_data(end_y);
  
  i2c1_stop_condition();

}

void ssd1306_update_display(void)
{
    ssd1306_set_resolution(
      SSD1306_CONFIGURATION_MIN_SEGMENT, 
      SSD1306_CONFIGURATION_MAX_SEGMENT, 
      SSD1306_CONFIGURATION_MIN_PAGE,
      SSD1306_CONFIGURATION_MAX_PAGE);


    i2c1_start();
    i2c1_slave_adress(SSD1306_ADDR);
    i2c1_data(SSD1306_CONTROL_BYTE_DATA_STREAM);

    dma_channel6_set_pma((uint8_t*)ssd1306_frame_buffer, FRAME_BUFFER_SIZE);

    dma_channel6_enable();
}



