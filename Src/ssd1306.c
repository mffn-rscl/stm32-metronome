/**
 * @file    ssd1306.c
 * @brief   SSD1306 128x32 OLED rendering and DMA upload implementation.
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The renderer uses a fixed 8x8 bitmap font for the digits 0-9 and for the
 * "BPM:" prefix. Glyphs are copied into the global frame buffer at page/column
 * offsets and the complete buffer is then streamed to the display through I2C1
 * and DMA1 Channel 6.
 *
 * @note
 * No dynamic allocation, printf or floating-point formatting is used. The BPM
 * conversion is performed with integer division and modulo, which keeps the
 * code deterministic and avoids pulling in newlib formatting routines.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#include "../Inc/ssd1306.h"
#include "../Inc/i2c_driver.h"
#include "../Inc/dma_driver.h"

/**
 * @brief   Global display frame buffer.
 *
 * @note
 * One byte represents eight vertical pixels in one page. The buffer is
 * zero-initialised by the C startup code.
 */
volatile uint8_t ssd1306_frame_buffer[FRAME_BUFFER_SIZE] = {0};

/**
 * @brief   8x8 bitmap font for the decimal digits.
 *
 * @details
 * Each row of the array is one glyph. Within a glyph, the eight bytes represent
 * successive columns, least-significant bit at the top. The table is
 * @c static @c const so that it is placed in flash and has internal linkage.
 */
static const uint8_t font8x8_digits[DIGITS_QUANTITY][BITS_PER_ONE_SYMBOL] =
{
  /* '0' */ {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00},
  /* '1' */ {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00},
  /* '2' */ {0x00, 0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00},
  /* '3' */ {0x00, 0x22, 0x41, 0x49, 0x49, 0x36, 0x00, 0x00},
  /* '4' */ {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00},
  /* '5' */ {0x00, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00},
  /* '6' */ {0x00, 0x3E, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00},
  /* '7' */ {0x00, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00},
  /* '8' */ {0x00, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
  /* '9' */ {0x00, 0x26, 0x49, 0x49, 0x49, 0x3E, 0x00, 0x00}
};

/**
 * @brief   8x8 bitmap font for the "BPM:" prefix.
 *
 * @details
 * Index 0 = 'B', 1 = 'P', 2 = 'M', 3 = ':', 4 = space. The table is
 * @c static @c const so that it is placed in flash and has internal linkage.
 */
static const uint8_t font8x8_bpm[SYMBOLS_QUANTITY][BITS_PER_ONE_SYMBOL] =
{
  /* 'B' */ {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00},
  /* 'P' */ {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00},
  /* 'M' */ {0x00, 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00, 0x00},
  /* ':' */ {0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00},
  /* ' ' */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/**
 * @brief   Copy one 8x8 glyph into the frame buffer.
 *
 * @param[out] dest Destination pointer inside @ref ssd1306_frame_buffer.
 * @param[in]  src  Source glyph in flash.
 *
 * @note
 * Exactly @ref BITS_PER_ONE_SYMBOL bytes are copied. The loop bound is a
 * compile-time constant, which allows the compiler to unroll it.
 *
 * @warning
 * No bounds checking is performed. The caller must guarantee that @p dest
 * points to at least eight writable bytes inside the frame buffer.
 */
static inline void copy_8bytes(volatile uint8_t *dest, const uint8_t *src)
{
  for (uint8_t i = 0U; i < BITS_PER_ONE_SYMBOL; i++)
  {
    dest[i] = src[i];
  }
}

/**
 * @brief   Send the SSD1306 initialisation command sequence.
 *
 * @note
 * The sequence follows the SSD1306 datasheet application note for a 128x32
 * panel with the internal charge pump enabled. All transfers are blocking and
 * timeout-protected.
 *
 * @note
 * The display is turned off first and on last to avoid visible start-up
 * artefacts.
 */
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

/**
 * @brief   Set the active column and page window.
 *
 * @note
 * The command is sent as a single I2C write with the command-stream control
 * byte. The controller latches the window and automatically advances the write
 * pointer in horizontal addressing mode.
 */
void ssd1306_set_resolution(uint8_t start_x, uint8_t end_x,
                            uint8_t start_y, uint8_t end_y)
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

/**
 * @brief   Start a DMA-backed frame-buffer upload.
 *
 * @note
 * The function programs the full 128x64 page range (0-7) and the full column
 * range (0-127), then starts DMA1 Channel 6. It returns before the transfer
 * completes; the DMA interrupt generates STOP.
 *
 * @note
 * The frame buffer must not be modified until the DMA completion interrupt has
 * fired. In this application the next modification is triggered by a button
 * interrupt, which is far slower than the I2C transfer.
 */
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

  dma_channel6_set_pma((uint8_t *)ssd1306_frame_buffer, FRAME_BUFFER_SIZE);

  dma_channel6_enable();
}

/**
 * @brief   Render the "BPM:" prefix into the frame buffer.
 *
 * @note
 * The prefix is placed on page @ref SSD1306_PAGE_VALUE starting at column
 * @ref SSD1306_COL_VALUE. The glyphs are copied from flash into SRAM.
 */
void draw_bpm_prefix(void)
{
  uint16_t offset = (uint16_t)((SSD1306_PAGE_VALUE * 128) + SSD1306_COL_VALUE);

  copy_8bytes(&ssd1306_frame_buffer[offset],      font8x8_bpm[0]); /* 'B' */
  copy_8bytes(&ssd1306_frame_buffer[offset + 8U],  font8x8_bpm[1]); /* 'P' */
  copy_8bytes(&ssd1306_frame_buffer[offset + 16U], font8x8_bpm[2]); /* 'M' */
  copy_8bytes(&ssd1306_frame_buffer[offset + 24U], font8x8_bpm[3]); /* ':' */
  copy_8bytes(&ssd1306_frame_buffer[offset + 32U], font8x8_bpm[4]); /* ' ' */
}

/**
 * @brief   Render a three-digit BPM value into the frame buffer.
 *
 * @note
 * The hundreds, tens and units digits are extracted with integer division and
 * modulo. Leading zeros are displayed, so 60 becomes "060".
 *
 * @note
 * The glyphs are placed 40 pixels to the right of the prefix origin.
 */
void set_current_bpm(uint8_t bpm_val)
{
  uint8_t hundreds = (uint8_t)((bpm_val / 100U) % 10U);
  uint8_t tens     = (uint8_t)((bpm_val / 10U) % 10U);
  uint8_t units    = (uint8_t)(bpm_val % 10U);

  uint16_t offset = (uint16_t)((SSD1306_PAGE_VALUE * 128) +
                               SSD1306_COL_VALUE + 40U);

  copy_8bytes(&ssd1306_frame_buffer[offset],       font8x8_digits[hundreds]);
  copy_8bytes(&ssd1306_frame_buffer[offset + 8U],  font8x8_digits[tens]);
  copy_8bytes(&ssd1306_frame_buffer[offset + 16U], font8x8_digits[units]);
}
