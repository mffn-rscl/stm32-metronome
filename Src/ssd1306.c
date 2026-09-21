#include "../Inc/ssd1306.h"
#include "../Inc/i2c_driver.h"
#include "../Inc/dma_driver.h"

 volatile  uint8_t ssd1306_frame_buffer[FRAME_BUFFER_SIZE] = {0}; 



const uint8_t font8x8_digits[DIGITS_QUANTITY][BITS_PER_ONE_SYMBOL] = {
    // '0'
    {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00}, 
    // '1'
    {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00}, 
    // '2'
    {0x00, 0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00}, 
    // '3'
    {0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00}, 
    // '4'
    {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00}, 
    // '5'
    {0x00, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00}, 
    // '6'
    {0x00, 0x3E, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00}, 
    // '7'
    {0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00}, 
    // '8'
    {0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00}, 
    // '9'
    {0x00, 0x26, 0x49, 0x49, 0x49, 0x3E, 0x00, 0x00}  
};



const uint8_t font8x8_bpm[SYMBOLS_QUANTITY][BITS_PER_ONE_SYMBOL] = {
    // 0: 'B'
    {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
    // 1: 'P'
    {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
    // 2: 'M'
    {0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00},
    // 3: ':'
    {0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00},
    // 4: ' ' 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};



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




/*FONT CONFIGURATION */
static inline void copy_8bytes(volatile uint8_t *dest, const uint8_t *src)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        dest[i] = src[i];
    }
}

void draw_bpm_prefix(void)
{
    uint16_t offset = (SSD1306_PAGE_VALUE * 128) + SSD1306_COL_VALUE;

    copy_8bytes(&ssd1306_frame_buffer[offset],      font8x8_bpm[0]); // 'B'
    copy_8bytes(&ssd1306_frame_buffer[offset + 8],  font8x8_bpm[1]); // 'P'
    copy_8bytes(&ssd1306_frame_buffer[offset + 16], font8x8_bpm[2]); // 'M'
    copy_8bytes(&ssd1306_frame_buffer[offset + 24], font8x8_bpm[3]); // ':'
    copy_8bytes(&ssd1306_frame_buffer[offset + 32], font8x8_bpm[4]); // ' '
}

void set_current_bpm(uint8_t bpm_val)
{
    uint8_t hundreds = (bpm_val / 100) % 10;
    uint8_t tens     = (bpm_val / 10) % 10;
    uint8_t units    = bpm_val % 10;

    uint16_t offset = (SSD1306_PAGE_VALUE * 128) + SSD1306_COL_VALUE + 40;

    copy_8bytes(&ssd1306_frame_buffer[offset],      font8x8_digits[hundreds]);
    copy_8bytes(&ssd1306_frame_buffer[offset + 8],  font8x8_digits[tens]);
    copy_8bytes(&ssd1306_frame_buffer[offset + 16], font8x8_digits[units]);
}


