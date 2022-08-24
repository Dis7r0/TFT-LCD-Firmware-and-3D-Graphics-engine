// Course: CMPE - 240
//Submitted by : Dhanush Babu
//SID: 015923882


#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include "lcd_ssp_Operations.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

extern uint8_t src_addr[SSP_BUFSIZE];
extern uint8_t dest_addr[SSP_BUFSIZE];

/*****************************************************************************
 ** Descriptions:        Set address window
 ** parameters:          Starting point (x0,y0), Ending point(x1,y1)
 ** Returned value:      None
 *****************************************************************************/



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

{

	writecommand(ST7735_CASET);

	writeword(x0);

	writeword(x1);

	writecommand(ST7735_RASET);

	writeword(y0);

	writeword(y1);

}

/*****************************************************************************
 ** Descriptions:        Fill LCD upto given co-ordinates
 ** parameters:          Starting point (x0,y0), Ending point(x1,y1) and color
 ** Returned value:      None
 *****************************************************************************/

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

	//int16_t i;

	int16_t width, height;

	width = x1 - x0 + 1;

	height = y1 - y0 + 1;

	setAddrWindow(x0, y0, x1, y1);

	writecommand(ST7735_RAMWR);

	write888(color, width * height);

}

/*****************************************************************************
 ** Descriptions:        Delay given to LCD in milliseconds
 ** parameters:          delay time in milliseconds
 ** Returned value:      None
 *****************************************************************************/

void lcddelay(int ms)

{

	int count = 24000;

	int i;

	for (i = count * ms; i--; i > 0)
		;

}

/*****************************************************************************
 ** Descriptions:        Initialize LCD and turn it ON
 ** parameters:          None
 ** Returned value:      None
 *****************************************************************************/
void lcd_init()

{

	int i;
	printf("LCD Demo Begins!!!\n");
	// Set pins P0.16, P0.21, P0.22 as output
	LPC_GPIO0->FIODIR |= (0x1 << 16);

	LPC_GPIO0->FIODIR |= (0x1 << 21);

	LPC_GPIO0->FIODIR |= (0x1 << 22);

	// Hardware Reset Sequence
	LPC_GPIO0->FIOSET |= (0x1 << 22);
	lcddelay(500);

	LPC_GPIO0->FIOCLR |= (0x1 << 22);
	lcddelay(500);

	LPC_GPIO0->FIOSET |= (0x1 << 22);
	lcddelay(500);

	// initialize buffers
	for (i = 0; i < SSP_BUFSIZE; i++) {

		src_addr[i] = 0;
		dest_addr[i] = 0;
	}

	// Take LCD display out of sleep mode
	writecommand(ST7735_SLPOUT);
	lcddelay(200);

	// Turn LCD display on
	writecommand(ST7735_DISPON);
	lcddelay(200);

}

/*  These contains function to perform SSP operations such as:
*  	1. SPIwrite
*  	2. Give commands signals to LCD
*  	3. Give Data to LCD
*  	4. Give color values LCD to display on the LCD*/

void spiwrite(uint8_t c)
{
	int pnum = 0;
	src_addr[0] = c;
	SSP_SSELToggle(pnum, 0);
	SSPSend(pnum, (uint8_t *) src_addr, 1);
	SSP_SSELToggle(pnum, 1);
}

void writecommand(uint8_t c)
{
	LPC_GPIO0->FIOCLR |= (0x1 << 21);
	spiwrite(c);
}

void writedata(uint8_t c)
{
	LPC_GPIO0->FIOSET |= (0x1 << 21);
	spiwrite(c);
}

void writeword(uint16_t c)
{
	uint8_t d;
	d = c >> 8;
	writedata(d);
	d = c & 0xFF;
	writedata(d);
}

void write888(uint32_t color, uint32_t repeat)
{

	uint8_t red, green, blue;
	int i;
	red = (color >> 16);
	green = (color >> 8) & 0xFF;
	blue = color & 0xFF;

	for (i = 0; i < repeat; i++)
	{
		writedata(red);
		writedata(green);
		writedata(blue);
	}
	//Command to invert the colors as the LCD is different
	//than sold by Adafruit
	//writecommand(ST7735_INVON);
}
