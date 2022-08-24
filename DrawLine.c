// Course: CMPE - 240
//Submitted by : Dhanush Babu
//SID: 015923882

#include "LPC17xx.h"                        /* LPC17xx definitions */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "DrawLine.h"
#include "ssp.h"
#include "lcd_ssp_Operations.h"

static int _height = ST7735_TFTHEIGHT;
static int _width = ST7735_TFTWIDTH;

/*****************************************************************************
 ** Descriptions:        Draw pixels according to given co-ordinates and color
 ** parameters:          Pixel Location (x0,y0) and color
 ** Returned value:      None
 *****************************************************************************/
void drawPixel(int16_t x, int16_t y, uint32_t color)
{

	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

		return;

	setAddrWindow(x, y, x + 1, y + 1);

	writecommand(ST7735_RAMWR);

	write888(color, 1);

}

/*****************************************************************************
 ** Descriptions:        Draw line function
 ** parameters:          Starting point (x0,y0), Ending point(x1,y1) and color
 ** Returned value:      None
 ****************************************************************************/

void drawline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

	int16_t slope = abs(y1 - y0) > abs(x1 - x0);

	if (slope) {

		swap(x0, y0);

		swap(x1, y1);

	}

	if (x0 > x1) {

		swap(x0, x1);

		swap(y0, y1);

	}

	int16_t dx, dy;

	dx = x1 - x0;

	dy = abs(y1 - y0);

	int16_t err = dx / 2;

	int16_t ystep;

	if (y0 < y1) {

		ystep = 1;

	}

	else {

		ystep = -1;

	}

	for (; x0 <= x1; x0++) {

		if (slope) {

			drawPixel(y0, x0, color);

		}

		else {

			drawPixel(x0, y0, color);

		}

		err -= dy;

		if (err < 0) {

			y0 += ystep;

			err += dx;

		}

	}

}
