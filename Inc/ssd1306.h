/**
 * @file    ssd1306.h
 * @brief   SSD1306 128x32 OLED display driver (I2C + DMA).
 * @author  Anton Popov
 * @date    2026-10-19
 * @version 1.0.0
 *
 * @details
 * The display module is an ER_OLEDM0.91_1x-I2C panel based on the SSD1306
 * controller. The driver owns a 1024-byte frame buffer in SRAM and renders an
 * 8x8 bitmap font into it. Frame-buffer uploads use I2C1 with DMA1 Channel 6 so
 * that the CPU is free immediately after the transfer is started.
 *
 * @note
 * The panel is 128 pixels wide and 32 pixels high, which corresponds to four
 * SSD1306 pages. The frame buffer is intentionally sized for eight pages
 * (1024 bytes); only the first four pages are visible on this panel.
 *
 * @warning
 * The frame buffer is shared with the DMA engine and must not be modified while
 * a transfer is in progress. The display driver is not re-entrant.
 *
 * @copyright
 * SPDX-License-Identifier: MIT
 */

#ifndef DEVICE_DRIVER_SSD1306
#define DEVICE_DRIVER_SSD1306

#include <string.h>
#include <stdint.h>

#include "../CMSIS/Device/stm32f103xb.h"

/**
 * @def     SSD1306_ADDR
 * @brief   7-bit I2C slave address of the SSD1306 controller.
 *
 * @note
 * The address is shifted left by the I2C driver before transmission.
 */
#define SSD1306_ADDR 0x3CU

/* ------------------------------------------------------------------------- */
/* Font configuration                                                        */
/* ------------------------------------------------------------------------- */

/**
 * @def     SSD1306_PAGE_VALUE
 * @brief   Display page used as the vertical origin for text rendering.
 */
#define SSD1306_PAGE_VALUE 3

/**
 * @def     SSD1306_COL_VALUE
 * @brief   Display column used as the horizontal origin for text rendering.
 */
#define SSD1306_COL_VALUE 32

/**
 * @def     DIGITS_QUANTITY
 * @brief   Number of decimal digit glyphs in @c font8x8_digits.
 */
#define DIGITS_QUANTITY 10

/**
 * @def     BITS_PER_ONE_SYMBOL
 * @brief   Number of bytes in one 8x8 glyph column set.
 */
#define BITS_PER_ONE_SYMBOL 8

/**
 * @def     SYMBOLS_QUANTITY
 * @brief   Number of glyphs in the "BPM:" prefix font.
 */
#define SYMBOLS_QUANTITY 5

/**
 * @def     FRAME_BUFFER_SIZE
 * @brief   Size of the display frame buffer in bytes (1 KB).
 *
 * @note
 * The SSD1306 is organised as 8 pages x 128 columns. A 128x32 panel uses the
 * first 512 bytes; the remaining space is reserved for a future 128x64 panel or
 * for double buffering.
 */
#define FRAME_BUFFER_SIZE 1024

/**
 * @brief   Global frame buffer shared between the renderer and DMA.
 *
 * @note
 * Declared @c volatile because the DMA engine reads it asynchronously while the
 * CPU may update it. The buffer is zero-initialised at startup.
 */
extern volatile uint8_t ssd1306_frame_buffer[FRAME_BUFFER_SIZE];

/* ------------------------------------------------------------------------- */
/* SSD1306 control bytes                                                     */
/* ------------------------------------------------------------------------- */

/**
 * @def     SSD1306_CONTROL_BYTE_COMMAND_STREAM
 * @brief   Control byte: all following bytes are commands.
 */
#define SSD1306_CONTROL_BYTE_COMMAND_STREAM 0x00U

/**
 * @def     SSD1306_CONTROL_BYTE_DATA_STREAM
 * @brief   Control byte: all following bytes are display data.
 */
#define SSD1306_CONTROL_BYTE_DATA_STREAM 0x40U

/**
 * @def     SSD1306_CONTROL_BYTE_SINGLE_COMMAND
 * @brief   Control byte: the next byte is a single command.
 */
#define SSD1306_CONTROL_BYTE_SINGLE_COMMAND 0x80U

/**
 * @def     SSD1306_CONTROL_BYTE_SINGLE_DATA
 * @brief   Control byte: the next byte is a single data byte.
 */
#define SSD1306_CONTROL_BYTE_SINGLE_DATA 0xC0U

/* ------------------------------------------------------------------------- */
/* SSD1306 initialisation commands                                           */
/* ------------------------------------------------------------------------- */

/**
 * @def     SSD1306_CONFIGURATION_TURN_DISPLAY_OFF
 * @brief   Display off command (0xAE).
 */
#define SSD1306_CONFIGURATION_TURN_DISPLAY_OFF 0xAEU

/**
 * @def     SSD1306_CONFIGURATION_MUX_RATIO_CMD
 * @brief   Set multiplex ratio command (0xA8).
 */
#define SSD1306_CONFIGURATION_MUX_RATIO_CMD 0xA8U

/**
 * @def     SSD1306_CONFIGURATION_MUX_RATIO_ARG
 * @brief   Multiplex ratio argument (0x3F = 64 rows).
 *
 * @note
 * The fitted panel is 32 pixels high; a 32-row configuration would use 0x1F.
 * The value is retained from the 128x64 reference initialisation and should be
 * reviewed if the panel is changed. See the hardware errata in @c README.md.
 */
#define SSD1306_CONFIGURATION_MUX_RATIO_ARG 0x3FU

/**
 * @def     SSD1306_CONFIGURATION_DISPLAY_OFFSET_CMD
 * @brief   Set display offset command (0xD3).
 */
#define SSD1306_CONFIGURATION_DISPLAY_OFFSET_CMD 0xD3U

/**
 * @def     SSD1306_CONFIGURATION_DISPLAY_OFFSET_ARG
 * @brief   Display offset argument (0x00 = no vertical offset).
 */
#define SSD1306_CONFIGURATION_DISPLAY_OFFSET_ARG 0x00U

/**
 * @def     SSD1306_CONFIGURATION_MEM_ADDR_MODE_CMD
 * @brief   Set memory addressing mode command (0x20).
 */
#define SSD1306_CONFIGURATION_MEM_ADDR_MODE_CMD 0x20U

/**
 * @def     SSD1306_CONFIGURATION_MEM_ADDR_MODE_HORIZ
 * @brief   Horizontal addressing mode.
 */
#define SSD1306_CONFIGURATION_MEM_ADDR_MODE_HORIZ 0x00U

/**
 * @def     SSD1306_CONFIGURATION_SET_START_LINE
 * @brief   Set display start line to zero (0x40).
 */
#define SSD1306_CONFIGURATION_SET_START_LINE 0x40U

/**
 * @def     SSD1306_CONFIGURATION_SEGMENT_REMAP
 * @brief   Map column 127 to SEG0 (0xA1).
 */
#define SSD1306_CONFIGURATION_SEGMENT_REMAP 0xA1U

/**
 * @def     SSD1306_CONFIGURATION_SCAN_DIRECTION
 * @brief   Scan COM pins in reverse order (0xC8).
 */
#define SSD1306_CONFIGURATION_SCAN_DIRECTION 0xC8U

/**
 * @def     SSD1306_CONFIGURATION_COM_PINS_CMD
 * @brief   Set COM pins hardware configuration command (0xDA).
 */
#define SSD1306_CONFIGURATION_COM_PINS_CMD 0xDAU

/**
 * @def     SSD1306_CONFIGURATION_COM_PINS_ARG
 * @brief   Alternative COM pin configuration with no left/right remap (0x12).
 */
#define SSD1306_CONFIGURATION_COM_PINS_ARG 0x12U

/**
 * @def     SSD1306_CONFIGURATION_CONTRAST_CMD
 * @brief   Set contrast command (0x81).
 */
#define SSD1306_CONFIGURATION_CONTRAST_CMD 0x81U

/**
 * @def     SSD1306_CONFIGURATION_CONTRAST_ARG
 * @brief   Contrast value (0x7F = mid-scale).
 */
#define SSD1306_CONFIGURATION_CONTRAST_ARG 0x7FU

/**
 * @def     SSD1306_CONFIGURATION_ENTIRE_DISPLAY_RAM
 * @brief   Display follows RAM contents (0xA4).
 */
#define SSD1306_CONFIGURATION_ENTIRE_DISPLAY_RAM 0xA4U

/**
 * @def     SSD1306_CONFIGURATION_SET_NORMAL_DISPLAY
 * @brief   Normal (non-inverted) display mode (0xA6).
 */
#define SSD1306_CONFIGURATION_SET_NORMAL_DISPLAY 0xA6U

/**
 * @def     SSD1306_CONFIGURATION_SET_OSC_FREQ_CMD
 * @brief   Set display clock divide ratio / oscillator frequency command (0xD5).
 */
#define SSD1306_CONFIGURATION_SET_OSC_FREQ_CMD 0xD5U

/**
 * @def     SSD1306_CONFIGURATION_SET_OSC_FREQ_ARG
 * @brief   Oscillator frequency argument (0x80 = default).
 */
#define SSD1306_CONFIGURATION_SET_OSC_FREQ_ARG 0x80U

/**
 * @def     SSD1306_CONFIGURATION_CHARGE_PUMP_CMD
 * @brief   Enable the internal charge pump command (0x8D).
 */
#define SSD1306_CONFIGURATION_CHARGE_PUMP_CMD 0x8DU

/**
 * @def     SSD1306_CONFIGURATION_CHARGE_PUMP_ARG
 * @brief   Charge pump enabled (0x14).
 */
#define SSD1306_CONFIGURATION_CHARGE_PUMP_ARG 0x14U

/**
 * @def     SSD1306_CONFIGURATION_TURN_DISPLAY_ON
 * @brief   Display on command (0xAF).
 */
#define SSD1306_CONFIGURATION_TURN_DISPLAY_ON 0xAFU

/* ------------------------------------------------------------------------- */
/* Byte-position registers                                                   */
/* ------------------------------------------------------------------------- */

/**
 * @def     SSD1306_CONFIGURATION_SET_COL_ADDR
 * @brief   Set column address range command (0x21).
 */
#define SSD1306_CONFIGURATION_SET_COL_ADDR 0x21U

/**
 * @def     SSD1306_CONFIGURATION_SET_ROW_ADDR
 * @brief   Set page address range command (0x22).
 */
#define SSD1306_CONFIGURATION_SET_ROW_ADDR 0x22U

/**
 * @def     SSD1306_CONFIGURATION_MAX_SEGMENT
 * @brief   Highest valid column index (127).
 */
#define SSD1306_CONFIGURATION_MAX_SEGMENT 127U

/**
 * @def     SSD1306_CONFIGURATION_MIN_SEGMENT
 * @brief   Lowest valid column index (0).
 */
#define SSD1306_CONFIGURATION_MIN_SEGMENT 0U

/**
 * @def     SSD1306_CONFIGURATION_MAX_PAGE
 * @brief   Highest page index written by @ref ssd1306_update_display().
 *
 * @note
 * Value 7 addresses all eight pages of a 128x64 panel. For the fitted 128x32
 * panel only pages 0-3 are visible; the extra pages are harmless but waste bus
 * bandwidth.
 */
#define SSD1306_CONFIGURATION_MAX_PAGE 7U

/**
 * @def     SSD1306_CONFIGURATION_MIN_PAGE
 * @brief   Lowest page index (0).
 */
#define SSD1306_CONFIGURATION_MIN_PAGE 0U

/* ------------------------------------------------------------------------- */
/* Public API                                                                */
/* ------------------------------------------------------------------------- */

/**
 * @brief   Initialise the SSD1306 controller.
 *
 * @note
 * Sends the complete command sequence required for a 128x32 panel: display off,
 * timing, addressing mode, segment/COM remap, contrast, charge pump and display
 * on. The function uses blocking I2C transfers with timeouts.
 *
 * @warning
 * Must be called after @ref i2c1_init() and @ref systick_init().
 */
void ssd1306_init(void);

/**
 * @brief   Program the active column and page window.
 *
 * @param[in] start_x First column (0-127).
 * @param[in] end_x   Last column (0-127).
 * @param[in] start_y  First page (0-7).
 * @param[in] end_y    Last page (0-7).
 *
 * @note
 * The window is used by @ref ssd1306_update_display() before the DMA upload.
 * Values outside the controller range are not validated by this function.
 *
 * @warning
 * The caller is responsible for ensuring that the requested window is valid for
 * the fitted panel (pages 0-3 for a 128x32 display).
 */
void ssd1306_set_resolution(uint8_t start_x, uint8_t end_x,
                            uint8_t start_y, uint8_t end_y);

/**
 * @brief   Upload the frame buffer to the display using I2C1 + DMA.
 *
 * @note
 * The function is non-blocking after the DMA channel is enabled: it returns
 * immediately while the transfer runs in the background. The DMA
 * transfer-complete interrupt generates the I2C STOP condition.
 *
 * @note
 * A new upload must not be started until the previous DMA transfer has
 * completed.
 */
void ssd1306_update_display(void);

/**
 * @brief   Render a three-digit BPM value into the frame buffer.
 *
 * @param[in] bpm_val BPM value in the range 0-255.
 *
 * @note
 * The value is rendered as hundreds, tens and units with leading zeros (for
 * example 60 is displayed as "060"). The glyphs are written at page
 * @ref SSD1306_PAGE_VALUE, column @ref SSD1306_COL_VALUE + 40.
 *
 * @warning
 * The function writes to the frame buffer but does not trigger a display
 * update. Call @ref ssd1306_update_display() afterwards.
 */
void set_current_bpm(uint8_t bpm_val);

/**
 * @brief   Render the "BPM:" prefix into the frame buffer.
 *
 * @note
 * The prefix occupies five 8x8 glyphs starting at @ref SSD1306_COL_VALUE on
 * page @ref SSD1306_PAGE_VALUE.
 *
 * @warning
 * Does not trigger a display update.
 */
void draw_bpm_prefix(void);

#endif /* DEVICE_DRIVER_SSD1306 */
