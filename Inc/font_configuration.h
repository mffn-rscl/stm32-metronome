#ifndef SSD1306_FONT_CONFIGURATION
#define SSD1306_FONT_CONFIGURATION

#include <string.h>


/*DISPLAY MATH */

#define SSD1306_PAGE_VALUE 3
#define SSD1306_COL_VALUE 32

void set_current_bpm(uint16_t bpm_val, uint8_t *framebuffer);


void draw_bpm_prefix(uint8_t *framebuffer);




#define DIGITS_QUANTITY 10
#define BITS_PER_ONE_SYMBOL 8
#define SYMBOLS_QUANTITY 5


/*END DISPLAY MATH*/



#endif /*SSD1306_FONT_CONFIGURATION*/
