
#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"


//#define  LCD_FONT
	 
//#define  LCD_IMAGE
	 
	 
	 
#define LCD_W     (240)
#define LCD_H     (400)

//LCD REG
//#define LCD_REG_HADDR 0x200
//#define LCD_REG_VADDR 0x201
//#define LCD_REG_DATA  0x202
#define LCD_REG_DATA  0x22

/* LCD color */
#define White          0xFFFF //白色
#define Black          0x0000 //黑色
#define Grey           0xF7DE //灰色
#define Blue           0x001F //蓝色
#define Blue2          0x051F //深蓝色
#define Red            0xF800 //红色
#define Magenta        0xF81F //品红
#define Green          0x07E0 //绿色
#define Cyan           0x7FFF //蓝绿色
#define Yellow         0xFFE0 //黄色




#define DRAW_BACK_COLOR   1
typedef struct 
{
  int16_t FColor;
  int16_t BColor;
  int32_t Mode;
} DrawMode, *pDrawMode;  




void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);

__inline void LCD_WriteRAM_Prepare(void);
__inline void LCD_REG_Select(uint16_t REG_Code);
__inline void LCD_WriteRAM(uint16_t RGB_Code);


void init_lcd(void);

void LCD_Clear(uint16_t Color);

void LCD_Area_Clear(uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t Color);
void PutPixel(int16_t x, int16_t y, uint16_t Color);
void LCD_DrawLineHV(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color, uint16_t Direction);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color);
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t Color);
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint16_t Color);

#ifdef LCD_FONT

void LCD_DrawString(uint32_t x, uint32_t y, const uint8_t *Str, DrawMode *Mode);
void LCD_DisplayNum(uint32_t x, uint32_t y, uint32_t Num, DrawMode *Mode);

#endif

#ifdef LCD_IMAGE

void LCD_Draw_Imag(uint16_t x1,uint16_t y1,uint16_t weight,uint16_t hight,const u8 *img);

#endif

#ifdef __cplusplus
}
#endif


#endif /* __LCD_H */


