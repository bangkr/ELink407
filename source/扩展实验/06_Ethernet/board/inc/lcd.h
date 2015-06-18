#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx_conf.h"
#include "fonts.h"
 
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色

#define LCD_RAM   *(vu16*)((u32)0x60020000)  //disp Data ADDR
#define LCD_REG   *(vu16*)((u32)0x60000000)	 //disp Reg  ADDR

#define LCD_WIDTH	  320
#define LCD_HEIGHT	  240

static sFONT *Efont;
   
void LCD_Init(void);
void LCD_Font(sFONT *font);
void LCD_Cursor(u16 Xpos, u16 Ypos);
void LCD_Window(u16 Xpos, u16 Ypos, u16 Height, u16 Width);
void LCD_Mode(u8 mode);

void LCD_Clear(u16 Color);
void LCD_Point(u16 xpos, u16 ypos,u16 color);
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Rect(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Circle(u16 Xpos, u16 Ypos, u16 Radius, u16 color);
void LCD_Fill(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);

void LCD_Char(u16 Xpos, u16 Ypos, u8 Ascii, u16 color);
void LCD_String(u16 xpos, u16 ypos, u8 *ptr, u16 color);
void LCD_Num(u16 x, u16 y, s32 num, u8 len, u16 color);

void LCD_LineConf(void);
void LCD_FSMCConf(void);
void LCD_WriteReg(u8 Reg, u16 Value);
u16  LCD_ReadReg(u8 Reg);
void LCD_WriteRAM(u16 RGB_Code);

#endif
