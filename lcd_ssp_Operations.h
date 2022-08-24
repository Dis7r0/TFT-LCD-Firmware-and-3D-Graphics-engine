// Course: CMPE - 240
// Submitted by: Harshil B Darji
// SID: 1641035

#ifndef LCD_SSP_OPERATIONS_H_
#define LCD_SSP_OPERATIONS_H_

#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29
#define ST7735_INVON 0x21

#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

// defining color values

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define PINK 0xCBC0FF
#define LIME 0x00FF00
#define CYAN 0x00FFFF
#define SILVER 0xC0C0C0
#define GREY 0x808080
#define ORANGE 0xFFA500
#define BROWN 0xA52A2A
#define MAROON 0x800000
#define YELLOW 0xFFFF00



void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void lcddelay(int ms);
void lcd_init();

/*  These contains function to perform SSP operations such as:
*  	1. SPIwrite
*  	2. Give commands signals to LCD
*  	3. Give Data to LCD
*  	4. Give color values LCD to display on the LCD*/

void spiwrite(uint8_t c);
void writecommand(uint8_t c);
void writedata(uint8_t c);
void writeword(uint16_t c);
void write888(uint32_t color, uint32_t repeat);

#endif /* LCD_SSP_OPERATIONS_H_ */

