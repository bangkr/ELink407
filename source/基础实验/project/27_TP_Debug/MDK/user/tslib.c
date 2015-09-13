
#include "stm32f4xx.h"

#include "stm32f4xx_it.h"
#include "tp.h"
#include "lcd.h"
#include "tslib.h"
#include "uart.h"
#include <stdio.h>



int Tp_Calibration(calibration_t *cal) 
{
	int j;
	float n, x, y, x2, y2, xy, z, zx, zy;
	float det, a, b, c, e, f, i;
	float scaling = 65536.0;

  // Get sums for matrix
	n = x = y = x2 = y2 = xy = 0.0;
	for(j=0;j<5;j++) {
		printf("%f,%f\r\n",cal->x[j]*1.0,cal->y[j]*1.0);
		n += (float)1.0;
		x += (float)cal->x[j];
		y += (float)cal->y[j];
		x2 += (float)(cal->x[j]*cal->x[j]);
		y2 += (float)(cal->y[j]*cal->y[j]);
		xy += (float)(cal->x[j]*cal->y[j]);
	}

  // Get determinant of matrix -- check if determinant is too small
	det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
	if(det < (float)0.1 && det > (float)-0.1) {
		printf("ts_calibrate: determinant is too small -- %f\n",det);
		return 0;
	}

  // Get elements of inverse matrix
	a = (x2*y2 - xy*xy)/det;
	b = (xy*y - x*y2)/det;
	c = (x*xy - y*x2)/det;
	e = (n*y2 - y*y)/det;
	f = (x*y - n*xy)/det;
	i = (n*x2 - x*x)/det;

  // Get sums for x calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->xfb[j];
		zx += (float)(cal->xfb[j]*cal->x[j]);
		zy += (float)(cal->xfb[j]*cal->y[j]);
	}

  // Now multiply out to get the calibration for framebuffer x coord
	cal->a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

  // Get sums for y calibration
	z = zx = zy = 0;
	for(j=0;j<5;j++) {
		z += (float)cal->yfb[j];
		zx += (float)(cal->yfb[j]*cal->x[j]);
		zy += (float)(cal->yfb[j]*cal->y[j]);
	}

  // Now multiply out to get the calibration for framebuffer y coord
	cal->a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
	cal->a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
	cal->a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

	printf("%f %f %f\n",(a*z + b*zx + c*zy),
				(b*z + e*zx + f*zy),
				(c*z + f*zx + i*zy));

  // If we got here, we're OK, so assign scaling to a[6] and return
	cal->a[6] = (int)scaling;
	return 1;

/*	
// This code was here originally to just insert default values
	for(j=0;j<7;j++) {
		c->a[j]=0;
	}
	c->a[1] = c->a[5] = c->a[6] = 1;
	return 1;
*/

}

void Tp_Put_Cross(int x, int y, unsigned colidx)
{
	LCD_DrawLine (x - 10, y, x - 2, y, colidx);
	LCD_DrawLine (x + 2, y, x + 10, y, colidx);
	LCD_DrawLine (x, y - 10, x, y - 2, colidx);
	LCD_DrawLine (x, y + 2, x, y + 10, colidx);
	
	LCD_DrawLine (x - 6, y - 9, x - 9, y - 9, colidx + 1);
	LCD_DrawLine (x - 9, y - 8, x - 9, y - 6, colidx + 1);
	LCD_DrawLine (x - 9, y + 6, x - 9, y + 9, colidx + 1);
	LCD_DrawLine (x - 8, y + 9, x - 6, y + 9, colidx + 1);
	LCD_DrawLine (x + 6, y + 9, x + 9, y + 9, colidx + 1);
	LCD_DrawLine (x + 9, y + 8, x + 9, y + 6, colidx + 1);
	LCD_DrawLine (x + 9, y - 6, x + 9, y - 9, colidx + 1);
	LCD_DrawLine (x + 8, y - 9, x + 6, y - 9, colidx + 1);
}

static void get_sample (calibration_t *cal,
			int index, int x, int y, char *name)
{
	static int last_x = -1, last_y;
	tp_point_t point;
	Tp_Put_Cross(x, y,  0xFFFE);
	
	while(1){		
		if (pen_flag){
			TpGetXY(&point);
			pen_flag = 0;
			break;
		}
	} 
	
	
	//getxy (&cal->x [index], &cal->y [index]);
	cal->x[index] = point.x;
	cal->y[index] = point.y;
	
	Tp_Put_Cross(x, y,  0xFFFE);
//	Delay (300);

	last_x = cal->xfb [index] = x;
	last_y = cal->yfb [index] = y;

	printf("%s : LCD X = %4d Y = %4d\n\r", name, cal->xfb [index], cal->yfb [index]);
	printf("%s : X = %4d Y = %4d\n\r", name, cal->x [index], cal->y [index]);
}


int ts_test(void)
{
	calibration_t cal;
	tp_point_t point;
	int cal_fd;
	char cal_buffer[256];
	char *tsdevice = NULL;
	char *calfile = NULL;
	unsigned int i;

	int xtemp,ytemp;
	int x,y;

	/* Clear the LCD */ 
	LCD_Clear(Blue);
	get_sample (&cal, 0, 50,        50,        "Top left");

	get_sample (&cal, 1, XRES - 50, 50,        "Top right");

	get_sample (&cal, 2, XRES - 50, YRES - 50, "Bot right");

	get_sample (&cal, 3, 50,        YRES - 50, "Bot left");

	get_sample (&cal, 4, XRES / 2,  YRES / 2,  "Center");

	if (Tp_Calibration (&cal)) {
		printf ("Calibration constants: ");
		for (i = 0; i < 7; i++) 
			printf("%d ", cal.a [i]);
		printf("\n\r");
		sprintf (cal_buffer,"%d %d %d %d %d %d %d",
			 cal.a[0], cal.a[1], cal.a[2],
			 cal.a[3], cal.a[4], cal.a[5], cal.a[6]);
		printf ("%d %d %d %d %d %d %d",
			 cal.a[1], cal.a[2], cal.a[0],
			 cal.a[4], cal.a[5], cal.a[3], cal.a[6]);
        i = 0;
	} 
	else {
		printf("Calibration failed.\n\r");
		i = -1;
	}
	while(1)
	{
		while(1){		
			if (pen_flag){
				
				TpGetXY(&point);
				LCD_Clear(White);
				pen_flag = 0;
				break;
			}
		}
		cal.x[0] = point.x;
		cal.y[0] = point.y;
		printf("before Calibration x = %d y=%d\n\r",cal.x[0],cal.y[0]);
	   		xtemp = cal.x[0];
			ytemp = cal.y[0];
			x = 	(int)(( cal.a[0] +
					cal.a[1]*xtemp + 
					cal.a[2]*ytemp ) / cal.a[6]);
			y =		(int)(( cal.a[3] +
					cal.a[4]*xtemp +
					cal.a[5]*ytemp ) / cal.a[6]);
		printf("after Calibration x = %d y=%d\n\r",x,y);
		LCD_DrawCircle(x, y, 5, Red);
	}

//	close_framebuffer();
	return i;
}
