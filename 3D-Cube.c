// Course: CMPE - 240
//Submitted by : Dhanush Babu
//SID: 015923882

#include "LPC17xx.h"
#include "ssp.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "lcd_ssp_Operations.h"
#include "DrawLine.h"

// Define Global variables
#define PORT_NUM 0
#define LOCATION_NUM 0
uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];
int colstart = 0;
int rowstart = 0;

// Define colors
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0xFFFFFF
#define BLUE 0x0000FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define YELLOW 0xFFFF00
#define SILVER 0xC0C0C0
#define LIME 0x00FF00
#define ORANGE 0xFFA500
#define MAROON 0x800000
#define FOREST 0x228B22
#define DARKGREEN 0x006400
#define LIGHTGREEN 0X90EE90
#define SEAGREEN 0X2E8B57
#define ORANGERED 0XFF4500
#define GOLD 0XFFD700
#define GRAY 0X808080
#define SHADOW 0X8A795D

struct coordinates
{
    int x;
    int y;
};

struct coordinate
{
    int x;
    int y;
    int z;
};

struct coordinates o1, o2, o3, o4, o5;

int xcamera = 200, ycamera = 200, zcamera = 200;
int elev = 10;
int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;
int cursor_x = 0, cursor_y = 0;

uint32_t colortext = GREEN, colorbg = YELLOW;
float textsize = 3;
int wrap = 1;
int round(float number)
{
    return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}

void drawVLine(int16_t x, int16_t y, int16_t h, uint32_t color)
{
    drawline(x, y, x, y + h, color);
}

void drawBackground(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    int16_t i;
    for (i = x; i < x + w; i++)
    {
        drawVLine(i, y, h, color);
    }
}

struct coordinates transfPipeline(int xw, int yw, int zw)
{
    int xDPrime, yDPrime, D = 100, l1 = 80, l2 = 50;
    double xPrime, yPrime, zPrime, theta, phi, rho;
    struct coordinates proj;

    theta = acos(xcamera / sqrt(pow(xcamera, 2) + pow(ycamera, 2)));
    phi = acos(zcamera / sqrt(pow(xcamera, 2) + pow(ycamera, 2) + pow(zcamera, 2)));
    rho = sqrt((pow(xcamera, 2)) + (pow(ycamera, 2)) + (pow(zcamera, 2)));
    xPrime = (yw * cos(theta)) - (xw * sin(theta));
    yPrime = (zw * sin(phi)) - (xw * cos(theta) * cos(phi)) - (yw * cos(phi) * sin(theta));
    zPrime = rho - (yw * sin(phi) * cos(theta)) - (xw * sin(phi) * cos(theta)) - (zw * cos(phi));
    xDPrime = xPrime * D / zPrime;
    yDPrime = yPrime * D / zPrime;
    xDPrime = l1 + xDPrime;
    yDPrime = l2 - yDPrime;
    proj.x = xDPrime;
    proj.y = yDPrime;
    return proj;
}

void drawCoords()
{
    struct coordinates lcd;
    int x1, y1, x2, y2, x3, y3, x4, y4;
    lcd = transfPipeline(0, 0, 0);
    x1 = lcd.x;
    y1 = lcd.y;
    lcd = transfPipeline(250, 0, 0);
    x2 = lcd.x;
    y2 = lcd.y;
    lcd = transfPipeline(0, 250, 0);
    x3 = lcd.x;
    y3 = lcd.y;
    lcd = transfPipeline(0, 0, 250);
    x4 = lcd.x;
    y4 = lcd.y;
    drawline(x1, y1, x2, y2, RED);
    drawline(x1, y1, x3, y3, BLUE);
    drawline(x1, y1, x4, y4, GREEN);
}

float diffusedReflection(struct coordinate Pi)
{
    struct coordinate Ps = {40, 60, 160};
    float red;
    float scaling = 2000, shift = 0.2;
    float distanceSqr = (pow((Ps.x - Pi.x), 2) + pow((Ps.y - Pi.y), 2) + pow((Ps.z - Pi.z), 2));
    float cosine = ((Ps.z - Pi.z) / sqrt(distanceSqr));
    float temp = cosine / distanceSqr;

    temp = (scaling * temp);
   // printf("temp %f\n", temp);
    temp = (temp < shift) ? shift : temp;
    red = (255 * 0.9 * temp);
    return red;
}

void fillShadow(struct coordinate startPt, struct coordinate endPt)
{

    struct coordinate temp;
    struct coordinates currPt;
    for (float i = startPt.y; i < endPt.y; i++)
    {
        for (float j = startPt.x; j < endPt.x; j++)
        {
            temp.x = j;
            temp.y = i;
            temp.z = 0;

            currPt = transfPipeline(temp.x, temp.y, temp.z);
            drawPixel(currPt.x, currPt.y, SHADOW);
        }
    }
}

void fillside(struct coordinate startPt, struct coordinate endPt, int size, int side)
{
    struct coordinate temp;
    struct coordinates currPt;
    if (side == 1)
    {
        for (float i = startPt.z; i < endPt.z; i++)
        {
            for (float j = startPt.y; j < endPt.y; j++)
            {
                temp.x = size;
                temp.y = j;
                temp.z = i;
                currPt = transfPipeline(temp.x, temp.y, temp.z);
                drawPixel(currPt.x, currPt.y, GREEN);
            }
        }
    }
    else if (side == 2)
    {
        for (float i = startPt.z; i < endPt.z; i++)
        {
            for (float j = startPt.x; j < endPt.x; j++)
            {
                temp.x = j;
                temp.y = size;
                temp.z = i;
                currPt = transfPipeline(temp.x, temp.y, temp.z);
                drawPixel(currPt.x, currPt.y, BLUE);
            }
        }
    }

    else if (side == 3)
    {
        uint32_t color;
        float r, g = 0, b = 0;
        for (float i = startPt.y; i < endPt.y; i++)
        {
            for (float j = startPt.x; j < endPt.x; j++)
            {
                temp.x = j;
                temp.y = i;
                temp.z = size + elev;
                r = diffusedReflection(temp);
                g = 0;
                b = 0;
                color = (((uint32_t)round(r)) << 16) + (((uint32_t)g) << 8) + ((uint32_t)b);
                currPt = transfPipeline(temp.x, temp.y, temp.z);
                drawPixel(currPt.x, currPt.y, color);
            }
        }
    }
}

uint32_t findColor(float x, float y, float Idiff1, float Idiff2, struct coordinates p1, struct coordinates p2)
{
    float newx, newy, ncolor;
    uint32_t color;
    float green, blue;

    newx = Idiff1 + (Idiff2 - Idiff1) * (x - p1.x) / (p2.x - p1.x);
    newy = Idiff1 + (Idiff2 - Idiff1) * (y - p1.y) / (p2.y - p1.y);
    ncolor = (newx + newy) / 2.0;
    green = 0;
    blue = 0;
    color = (((uint32_t)round(ncolor)) << 16) + (((uint32_t)green) << 8) + ((uint32_t)blue);

    return color;
}

void DDA_line(struct coordinates p1, struct coordinates p2, float Idiff1, float Idiff2)
{
    uint32_t color;

    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xIncrement = dx / (float)steps;
    float yIncrement = dy / (float)steps;

    float X = p1.x;
    float Y = p1.y;
    for (int i = 0; i < steps; i++)
    {
        color = findColor(X, Y, Idiff1, Idiff2, p1, p2);
        drawPixel(X, Y, color);
        X += xIncrement;
        Y += yIncrement;
    }
}

void draw3DCube(int cube_size)
{
    struct coordinates c1, c2, c3, c4, c5, c6, c7;
    int pt = 0;
    struct coordinate cubeDimension = {pt, pt, (cube_size + pt + elev)}; //(0,0,100)
    float Idiff1, Idiff2, Idiff3, Idiff4;
    struct coordinate t1, t2, t3, t4, t5, t6, t7;
    t1 = cubeDimension;
    Idiff1 = diffusedReflection(t1);
    c1 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);
    cubeDimension.x = (cube_size + pt); //(100,0,100)
    t2 = cubeDimension;
    Idiff2 = diffusedReflection(t2);
    c2 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    cubeDimension.y = (cube_size + pt); //(100,100,110)
    t3 = cubeDimension;
    Idiff3 = diffusedReflection(t3);
    c3 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    cubeDimension.x = pt; //(0,100,100)
    t4 = cubeDimension;
    Idiff4 = diffusedReflection(t4);
    c4 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    cubeDimension.x = (cube_size + pt); //(100,0,0)
    cubeDimension.y = pt;
    cubeDimension.z = pt + elev;
    t5 = cubeDimension;
    c5 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    cubeDimension.y = (cube_size + pt); //(100,100,0)
    t6 = cubeDimension;
    c6 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    cubeDimension.x = pt; //(0,100,0)
    t7 = cubeDimension;
    c7 = transfPipeline(cubeDimension.x, cubeDimension.y, cubeDimension.z);

    DDA_line(c1, c2, Idiff1, Idiff2);
    DDA_line(c2, c3, Idiff2, Idiff3);
    DDA_line(c3, c4, Idiff3, Idiff4);
    DDA_line(c4, c1, Idiff4, Idiff1);

    drawline(c2.x, c2.y, c5.x, c5.y, BLUE);
    drawline(c5.x, c5.y, c6.x, c6.y, BLUE);
    drawline(c6.x, c6.y, c3.x, c3.y, BLUE);
    drawline(c6.x, c6.y, c7.x, c7.y, BLUE);
    drawline(c7.x, c7.y, c4.x, c4.y, BLUE);

    fillside(t5, t3, cube_size, 1); //Left fill
    fillside(t7, t3, cube_size, 2); //right fill
    fillside(t1, t3, cube_size, 3); //top surface
}

float lambda_calc(float zi, float zs)
{
    return -zi / (zs - zi);
}

void drawShadow(double cube_size, struct coordinate ps)
{
    float l1, l2, l3, l4;
    struct coordinates s1, s2, s3, s4;
    struct coordinate xp1, xp2, xp3, xp4;
    int pt = 0;
    struct coordinate p1 = {pt, pt, (cube_size + pt + elev)};                             //p1
    struct coordinate p2 = {(cube_size + pt), pt, (cube_size + pt + elev)};               //p2
    struct coordinate p3 = {(cube_size + pt), (cube_size + pt), (cube_size + pt + elev)}; //p4
    struct coordinate p4 = {pt, (cube_size + pt), (cube_size + pt + elev)};               //p3
    struct coordinates psp = transfPipeline(ps.x, ps.y, ps.z);
    l1 = lambda_calc(p1.z, ps.z);
    l2 = lambda_calc(p2.z, ps.z);
    l3 = lambda_calc(p3.z, ps.z);
    l4 = lambda_calc(p4.z, ps.z);
    xp1.x = p1.x + l1 * (ps.x - p1.x);
    xp1.y = p1.y + l1 * (ps.y - p1.y);
    xp1.z = p1.z + l1 * (ps.z - p1.z);

    xp2.x = p2.x + l2 * (ps.x - p2.x);
    xp2.y = p2.y + l2 * (ps.y - p2.y);
    xp2.z = p2.z + l2 * (ps.z - p2.z);

    xp3.x = p3.x + l3 * (ps.x - p3.x);
    xp3.y = p3.y + l3 * (ps.y - p3.y);
    xp3.z = p3.z + l3 * (ps.z - p3.z);

    xp4.x = p4.x + l4 * (ps.x - p4.x);
    xp4.y = p4.y + l4 * (ps.y - p4.y);
    xp4.z = p4.z + l4 * (ps.z - p4.z);

    s1 = transfPipeline(xp1.x, xp1.y, xp1.z);
    s2 = transfPipeline(xp2.x, xp2.y, xp2.z);
    s3 = transfPipeline(xp3.x, xp3.y, xp3.z);
    s4 = transfPipeline(xp4.x, xp4.y, xp4.z);

    drawline(s1.x, s1.y, s2.x, s2.y, DARKBLUE);
    drawline(s2.x, s2.y, s3.x, s3.y, DARKBLUE);
    drawline(s3.x, s3.y, s4.x, s4.y, DARKBLUE);
    drawline(s4.x, s4.y, s1.x, s1.y, DARKBLUE);

    o1 = s1; o2 = s2; o3 = s3; o4 = s4; o5 = psp;

    fillShadow(xp1, xp3);
}

void drawTree(uint32_t color, int startPoint, int size)
{
    int i = 0, angle;
    struct coordinates lcd;
    int tree_branch[3][3] = {{startPoint, startPoint + 40, 0.5 * size + 30}, {startPoint, startPoint + 40, 0.3 * size + 30}, {startPoint, startPoint + 40, 0.8 * size + 30}};
    while (i < 1)
    {
        int x0, y0, y1, x1, xp0, xp1, yp0, yp1;
        angle = startPoint + size;
        x0 = tree_branch[i][0];
        x1 = tree_branch[i][1];
        y0 = tree_branch[i][2] - 20;
        y1 = y0;
        i++;
        lcd = transfPipeline(y0, angle, x0);
        xp0 = lcd.x;
        yp0 = lcd.y;
        lcd = transfPipeline(y1, angle, x1);
        xp1 = lcd.x;
        yp1 = lcd.y;
        drawline(xp0, yp0, xp1, yp1, 0x69b4FF);                         //level 0 straight line
        drawline((xp0 + 1), (yp0 + 1), (xp1 + 1), (yp1 + 1), 0x69b4FF); //level 0 straight line
        drawline((xp0 - 1), (yp0 - 1), (xp1 - 1), (yp1 - 1), 0x69b4FF); //level 0 straight line

        int it = 0;
        for (it = 0; it < 3; it++)
        {
            int16_t x2 = (0.6 * (x1 - x0)) + x1; // length of level 1 = 0.8 of previous level
            int16_t y2 = y1;
            lcd = transfPipeline(y2, angle, x2);
            int xp2 = lcd.x;
            int yp2 = lcd.y;
            //			drawline(xp1, yp1, xp2, yp2,color);	//level 1 straight line

            //for right rotated angle 30 degree
            int16_t xr = ((0.134 * x1) + (0.866 * x2) - (0.5 * y2) + (0.5 * y1));
            int16_t yr = ((0.5 * x2) - (0.5 * x1) + (0.866 * y2) - (0.866 * y1) + y1);
            lcd = transfPipeline(yr, angle, xr);
            int xpr = lcd.x;
            int ypr = lcd.y;

            //for left rotated angle 30 degree
            int16_t xl = ((0.134 * x1) + (0.866 * x2) + (0.5 * y2) - (0.5 * y1));
            int16_t yl = ((0.5 * x1) - (0.5 * x2) + (0.134 * y2) + (0.866 * y1));
            lcd = transfPipeline(yl, angle, xl);
            int xpl = lcd.x;
            int ypl = lcd.y;

            drawline(xp1, yp1, xpr, ypr, DARKGREEN);
            drawline(xp1, yp1, xpl, ypl, DARKGREEN);

            //for branches on right rotated branch angle 30 degree
            int16_t xrLen = sqrt(pow((xr - x1), 2) + pow((yr - y1), 2)); //length of right branch
            int16_t xrImag = (0.8 * xrLen) + xr;                         //imaginary vertical line x coordinate, y= yr
            int16_t xr1 = ((0.134 * xr) + (0.866 * xrImag) - (0.5 * yr) + (0.5 * yr));
            int16_t yr1 = ((0.5 * xrImag) - (0.5 * xr) + (0.866 * yr) - (0.866 * yr) + yr);
            lcd = transfPipeline(yr1, angle, xr1);
            int xpr1 = lcd.x;
            int ypr1 = lcd.y;

            //for right branch
            int16_t xrr, xrl, yrr, yrl;
            xrr = ((0.134 * xr) + (0.866 * xr1) - (0.5 * yr1) + (0.5 * yr));
            yrr = ((0.5 * xr1) - (0.5 * xr) + (0.866 * yr1) - (0.866 * yr) + yr);
            lcd = transfPipeline(yrr, angle, xrr);
            int xprr = lcd.x;
            int yprr = lcd.y;

            //for left branch
            xrl = ((0.134 * xr) + (0.866 * xr1) + (0.5 * yr1) - (0.5 * yr));
            yrl = ((0.5 * xr) - (0.5 * xr1) + (0.134 * yr) + (0.866 * yr1));
            lcd = transfPipeline(yrl, angle, xrl);
            int xprl = lcd.x;
            int yprl = lcd.y;
            //for branches on left rotated branch angle 30 degree
            int16_t xlImag = (0.8 * xrLen) + xl; //imaginary vertical line x coordinate, y= yr
            int16_t xl1 = ((0.134 * xl) + (0.866 * xlImag) + (0.5 * yl) - (0.5 * yl));
            int16_t yl1 = ((0.5 * xl) - (0.5 * xlImag) + (0.134 * yl) + (0.866 * yl));
            lcd = transfPipeline(yl1, angle, xl1);
            int xpl1 = lcd.x;
            int ypl1 = lcd.y;
            //for right branch
            int16_t xlr, xll, ylr, yll;
            xlr = ((0.134 * xl) + (0.866 * xl1) - (0.5 * yl1) + (0.5 * yl));
            ylr = ((0.5 * xl1) - (0.5 * xl) + (0.866 * yl1) - (0.866 * yl) + yl);
            lcd = transfPipeline(ylr, angle, xlr);
            int xplr = lcd.x;
            int yplr = lcd.y;
            //for left branch
            xll = ((0.134 * xl) + (0.866 * xl1) + (0.5 * yl1) - (0.5 * yl));
            yll = ((0.5 * xl) - (0.5 * xl1) + (0.134 * yl) + (0.866 * yl1));
            lcd = transfPipeline(yll, angle, xll);
            int xpll = lcd.x;
            int ypll = lcd.y;
            drawline(xpr, ypr, xpr1, ypr1, DARKGREEN);
            drawline(xpr, ypr, xprr, yprr, DARKGREEN);
            drawline(xpr, ypr, xprl, yprl, DARKGREEN);
            drawline(xpl, ypl, xpl1, ypl1, DARKGREEN);
            drawline(xpl, ypl, xplr, yplr, DARKGREEN);
            drawline(xpl, ypl, xpll, ypll, DARKGREEN);

            x0 = x1;
            x1 = x2;
        }
    }
}

void draw_HorizontalLine(int16_t x, int16_t y, int16_t width, uint32_t color)
{
    drawline(x, y, x + width - 1, y, color);
}
void draw_letter_D(uint32_t color,int start_point, int size)
{
	int i = 0, angle=0;
	    struct coordinates lcd;

	    int x0,x1,x2,x3,x4,x5,y0,y1,y2,y3,y4,y5;
	    x0 = start_point + 10;
	    y0 = start_point + 30;
	    //y0 = start_point + 50;

	    lcd = transfPipeline(y0,x0,110);
	    int xp0 = lcd.x;
	    int yp0 = lcd.y;
	    x1 = x0 + 40;
	    y1 = y0;
	    lcd = transfPipeline(y1,x1,110);
	    int xp1 = lcd.x;
	    int yp1 = lcd.y;
	    x2 = x1 + 40;
	    y2 = y1;
	    //y2 = y1 + 20;
	    lcd = transfPipeline(y2,x2,110);
	    int xp2 = lcd.x;
	    int yp2 = lcd.y;
	    x3 = x2;
	    y3 = y2 + 30;
	    //y3 = y2 - 40;
	    lcd = transfPipeline(y3,x3,110);
	    int xp3 = lcd.x;
	    int yp3 = lcd.y;
	    x4 = x1;
	    y4 = y1 + 30;
	    lcd = transfPipeline(y4,x4,110);
	    int xp4 = lcd.x;
	    int yp4 = lcd.y;
	    x5 = x0;
	    y5 = y0 + 30;
	    lcd = transfPipeline(y5,x5,110);
	    int xp5 = lcd.x;
	    int yp5 = lcd.y;
	    drawline(xp0, yp0, xp1, yp1, DARKGREEN);
	   	drawline(xp1, yp1, xp2, yp2, DARKGREEN);
	   	drawline(xp2, yp2, xp3, yp3, DARKGREEN);
	   	drawline(xp4, yp4, xp5, yp5, DARKGREEN);
	   	drawline(xp5, yp5, xp0, yp0, DARKGREEN);
	   	drawline(xp3, yp3, xp4, yp4, DARKGREEN);
}

void drawSquare(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color, uint8_t level, float lambda)
{

	for(int i = 0 ; i < level; i++)
	{
		drawline(x0, y0, x1, y1, color);
		drawline(x1, y1, x2, y2, color);
		drawline(x2, y2, x3, y3, color);
		drawline(x3, y3, x0, y0, color);

		x0 = x0 + lambda * (x1 - x0);
		y0 = y0 + lambda * (y1 - y0);
		x1 = x1 + lambda * (x2 - x1);
		y1 = y1 + lambda * (y2 - y1);
		x2 = x2 + lambda * (x3 - x2);
		y2 = y2 + lambda * (y3 - y2);
		x3 = x3 + lambda * (x0 - x3);
		y3 = y3 + lambda * (y0 - y3);
	}
}

void draw_rotating_squares(int start_point, int size) {
	float lambda = 0.8;
	int color[] = {LIGHTBLUE, DARKBLUE, RED, WHITE, BLUE, YELLOW, RED, PURPLE};
	 for(int i = 0; i < 8; i++)
	 {
		 struct coordinates lcd;
		 int x0,y0, x1, y1, x2, y2, x3, y3, len, cindex;
		 x0 = rand() % 110;
		 y0 = rand() % 100;
		 len = rand() % 100;
//		 if ((x0 + len) > 100)
//			 len = 100 - x0;
//		 if ((y0 + len) > 100)
//		 		len = 100 - y0;
		 if ((x0 - len) < 10)
			 len = x0 - 10;
		 if ((y0 - len) < 0)
			 len = y0;
		 cindex = rand() % 8;

		 x1 = x0;
		 y1 = y0 - len;
		 x2 = x0 - len;
		 y2 = y1;
		 x3 = x2;
		 y3 = y0;

		 lcd = transfPipeline(100,y0,x0);
		 x0 = lcd.x;
		 y0 = lcd.y;
		 lcd = transfPipeline(100,y1,x1);
		 x1 = lcd.x;
		 y1 = lcd.y;
		 lcd = transfPipeline(100,y2,x2);
		 x2 = lcd.x;
		 y2 = lcd.y;
		 lcd = transfPipeline(100,y3,x3);
		 x3 = lcd.x;
		 y3 = lcd.y;

		 drawSquare(x0, y0, x1, y1, x2, y2, x3, y3, color[cindex], 5, lambda);
		 lcddelay(50);

	 }
}

int main(void)
{
    uint32_t portnum = PORT_NUM;
    if (portnum == 0)
        SSP0Init();
    else if (portnum == 1)
        SSP1Init();

    for (int i = 0; i < SSP_BUFSIZE; i++)
    {
        src_addr[i] = (uint8_t)i;
        dest_addr[i] = 0;
    }

    lcd_init();
    drawBackground(0, 0, _width, _height, WHITE);
    drawCoords();
    int startPoint = 0;
    int size = 100;
    double x[8] = {startPoint, (startPoint + size), (startPoint + size), startPoint, startPoint, (startPoint + size), (startPoint + size), startPoint};
    double y[8] = {startPoint, startPoint, startPoint + size, startPoint + size, startPoint, startPoint, (startPoint + size), (startPoint + size)};
    double z[8] = {startPoint, startPoint, startPoint, startPoint, (startPoint + size), (startPoint + size), (startPoint + size), (startPoint + size)};

    struct coordinate pointLightSource = {40, 60, 220};
    drawCoords();
    drawShadow(size, pointLightSource);
    draw3DCube(size);
    lcddelay(10);
    diffusedReflection(pointLightSource);
    drawTree(0x0066CC00, startPoint, size);
    draw_letter_D(0x0066CC00, startPoint, size);
    draw_rotating_squares(startPoint, size);

    //Draw lines showing point light source to shadow
    drawline(o1.x, o1.y, o5.x, o5.y, DARKBLUE);
    drawline(o2.x, o2.y, o5.x, o5.y, DARKBLUE);
    drawline(o3.x, o3.y, o5.x, o5.y, DARKBLUE);
    drawline(o4.x, o4.y, o5.x, o5.y, DARKBLUE);

    return 0;
}
