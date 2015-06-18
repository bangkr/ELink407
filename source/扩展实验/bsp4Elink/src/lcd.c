
#include <stdio.h>

#include "lcd.h"
#include "font.h"
#include "stm32f4xx_it.h"
#include "delay.h"


/* Note: LCD /CS is NE3 - Bank 3 of NOR/SRAM Bank 1~4 */
#define LCD_BASE           ((uint32_t)(0x60000000))

#define LCD_CMD_ADDR       ((uint32_t)(0x60020000))

#define LCD_CMD_REG        (*( uint16_t *)LCD_CMD_ADDR)
#define LCD_RAM_REG        (*( uint16_t *)LCD_CMD_ADDR)

#define LCD_IDX_REG        (*((__IO uint16_t *)LCD_BASE))


/**
 * @brief  LCD 用GPIO配置
 * @param  None
 * @retval None
 */
void LCD_CtrlLinesConfig(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOD, GPIOE, and AFIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	/*-- GPIO Configuration ------------------------------------------------------*/
	/* SRAM Data lines,  NOE and NWE configuration */
	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 |
		GPIO_Pin_0 | GPIO_Pin_1 |   GPIO_Pin_7 |
		GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |  GPIO_Pin_11 |
		GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
		GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
		GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);


	/* NE1 configuration */

	//GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*PD13(LCD_BL) configuration*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOD, GPIO_Pin_13);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	{
		int i;
		GPIO_ResetBits(GPIOD, GPIO_Pin_3);
		for (i = 0; i < 1000000; i++)
		{
			;
		}
		GPIO_SetBits(GPIOD, GPIO_Pin_3);
	}
	 

}

/**
 * @brief  为LCD配置 FSMC 
 * @param  无
 * @retval 无
 */
void LCD_FSMCConfig(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;

	/* Enable FSMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* FSMC_Bank1_NORSRAM_1 configuration */
	p.FSMC_AddressSetupTime = 5;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 9;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;
	
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   

	/* Enable FSMC NOR/SRAM Bank1 */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}



/**
 * @brief  向LCD寄存器写数据
 * @param  LCD_Reg 寄存器ID
 * @param  LCD_RegValue 寄存器内容
 * @retval 无
 */
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
	/* Write 16-bit Index, then Write Reg */
	LCD_IDX_REG = LCD_Reg;

	/* Write 16-bit Reg */
	LCD_CMD_REG = LCD_RegValue;
}


/**
 * @brief  选择LCD寄存器
 * @param[in]  REG_Code 像素点的颜色
 * @retval None
 */
__inline void LCD_REG_Select(uint16_t REG_Code)
{
	LCD_IDX_REG = REG_Code;
}



/**
 * @brief  使LCD处于接收显示数据的状态
 * @param  无
 * @retval 无
 */
__inline void LCD_WriteRAM_Prepare(void)
{
	LCD_IDX_REG =  LCD_REG_DATA;
}

/**
 * @brief  向LCD显存写入显示数据
 * @param  无
 * @retval 无
 */
__inline void LCD_WriteRAM(uint16_t RGB_Code)
{
	LCD_RAM_REG = RGB_Code;
}

/**
 * @brief  LCD初始化
 * @param  无
 * @retval 无
 */

//unsigned short id;
//unsigned short id2;


void init_lcd(void)
{ 
	/*配置LCD 用到的GPIO */
	LCD_CtrlLinesConfig();

	/* Configure the FSMC Parallel interface */
	LCD_FSMCConfig();
	delay_ms(10); /*delay 10 ms*/

	//----Power on sequence----------
	//********Start Initial Sequence******//       

	//LCD_IDX_REG = 0;	
	//id = LCD_CMD_REG;
	//id2 = LCD_CMD_REG;
	
	
	
	LCD_WriteReg(0x001A,0x0002);
	LCD_WriteReg(0x001B,0x0088);
	
	//****VCOM offset**///
	LCD_WriteReg(0x0023,0x0000);
	LCD_WriteReg(0x0024,0x0064);
	LCD_WriteReg(0x0025,0x0015);
	LCD_WriteReg(0x002D,0x0003);
	delay_ms(10);
	
	//****OPON**//
	LCD_WriteReg(0x00E8,0x0060);

	//Power on Setting
	LCD_WriteReg(0x0018,0x0004);
	LCD_WriteReg(0x0019,0x0001);
	LCD_WriteReg(0x0001,0x0000);
	LCD_WriteReg(0x001F,0x0088);
	
	delay_ms(10);
	LCD_WriteReg(0x1F,0x80);

	delay_ms(10);
	LCD_WriteReg(0x1F,0x90);

	delay_ms(10);
	LCD_WriteReg(0x1F,0xD0);
	delay_ms(10);

	//262k/65k color selection
	LCD_WriteReg(0x17,0x05);

	//SET PANEL
	LCD_WriteReg(0x36,0x03);
	LCD_WriteReg(0x29,0x31);
	LCD_WriteReg(0x71,0x1A);

	//Gamma 2.2 Setting
	LCD_WriteReg(0x40,0x00);
	LCD_WriteReg(0x41,0x45);
	LCD_WriteReg(0x42,0x45);
	LCD_WriteReg(0x43,0x04);

	LCD_WriteReg(0x44,0x00);
	LCD_WriteReg(0x45,0x08);
	LCD_WriteReg(0x46,0x23);
	LCD_WriteReg(0x47,0x23);

	LCD_WriteReg(0x48,0x77);
	LCD_WriteReg(0x49,0x40);
	LCD_WriteReg(0x4A,0x04);
	LCD_WriteReg(0x4B,0x00);
	LCD_WriteReg(0x4C,0x88);
	LCD_WriteReg(0x4D,0x88);
	LCD_WriteReg(0x4E,0x88);

	LCD_WriteReg(0x02,0x00);
	LCD_WriteReg(0x03,0x00);
	
	LCD_WriteReg(0x04,0x00);
	LCD_WriteReg(0x05,0xEF);

	LCD_WriteReg(0x06,0x00);
	LCD_WriteReg(0x07,0x00);
	
	LCD_WriteReg(0x08,0x01);
	LCD_WriteReg(0x09,0x8F);


	//Display ON Setting
	LCD_WriteReg(0x28,0x38); //GON=1, DTE=1, D=10
	delay_ms(40);
	LCD_WriteReg(0x28,0x3F); //3//3c//GON=1, DTE=1, D=11

	LCD_REG_Select(0x22);


	//LCD_WriteReg(0x16,0xA1); //填充方向 0xa1色彩正常，横向   0xaa R-B交换，横向
	//LCD_WriteRAM_Prepare();
}

/**
 * @brief  开启LCD显示
 * @param  无
 * @retval 无
 */
void LCD_DisplayOn(void)
{
	/* Display On */
	LCD_WriteReg(0x07, 0x0173); /* 262K color and display ON */
}

/**
 * @brief  禁止LCD显示
 * @param  无
 * @retval 无
 */
void LCD_DisplayOff(void)
{
	/* Display Off */
	LCD_WriteReg(0x7, 0x0); 
}




/**
 * @brief  根据x，y的坐标设置LCD显存位置
 * @param[in]  Xpos  x坐标
 * @param[in]  Ypos  y坐标
 * @retval 无
 */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WriteReg(0x02, ((Xpos>>8)&0xFF));
	LCD_WriteReg(0x03, (Xpos&0xFF));

	LCD_WriteReg(0x06, ((Ypos>>8)&0xFF));
	LCD_WriteReg(0x07, (Ypos&0xFF));
}

/**
 * @brief 在指定位置画一个点
 * @param[in]  x x坐标
 * @param[in]  y y坐标
 * @param[in]  Color RGB颜色值
 * @retval 无
 */
void PutPixel(int16_t x, int16_t y, uint16_t Color)
{ 
	if(y < 0 || y > (LCD_H - 1) || x < 0 || x > (LCD_W - 1))
	{
		return;
	}

	//设置光标位置 
	LCD_SetCursor(x,y);
	LCD_WriteRAM_Prepare(); 
	LCD_WriteRAM(Color);
}

/**
 * @brief 在指定位置画一个点
 * @param[in]  X x坐标
 * @param[in]  Y y坐标
 * @param[in]  Color RGB颜色值
 * @retval 无
 */
void LCD_DrawPoint(uint16_t X, uint16_t Y, uint16_t Color)
{
#if 0
	// Write 16-bit Index, then Write Reg
	LCD_IDX_REG  = LCD_REG_HADDR;
	// Write 16-bit Reg 
	LCD_CMD_REG = X;

	// Write 16-bit Index, then Write Reg 
	LCD_IDX_REG = LCD_REG_VADDR;
	// Write 16-bit Reg 
    LCD_CMD_REG = Y;

	//开始写入GRAM
	LCD_IDX_REG = LCD_REG_DATA;
	LCD_RAM_REG = Color;
#else
	//设置光标位置 
	LCD_SetCursor(X,Y);
	LCD_WriteRAM_Prepare(); 
	LCD_WriteRAM(Color);
#endif
} 


/**
 * @brief 设置显示区域（窗口）
 * @param[in]  x1,y1  窗口左上角坐标
 * @param[in]  x2,y2  窗口右下角坐标
 * @retval 无
 */
#if 0
void LCD_SetDisplayWindow(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_WriteReg(0x210, x1 & 0x00ff);
	LCD_WriteReg(0x211, x2 & 0x00ff);

	LCD_WriteReg(0x212, y1 & 0x01ff);
	LCD_WriteReg(0x213, y2 & 0x01ff);
}
#endif

/**
 * @brief  清屏
 * @param  无
 * @retval 无
 */
void LCD_Clear(uint16_t Color)
{
	uint32_t index = 0;

	LCD_SetCursor(0x00, 0x00); 
	LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
	for(index = 0; index < LCD_W * LCD_H; index++)
	{
		LCD_RAM_REG = Color;
	}  
}


/**
 * @brief  画水平或者垂直线
 * @param[in] Xpos, Ypos 起点坐标
 * @param[in] Length 线的长度
 * @param[in] Color 颜色
 * @param[in] Direction 水平/垂直，0 水平，1垂直
 * @retval 无
 */
void LCD_DrawLineHV(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Color, uint16_t Direction)
{
	uint32_t i = 0;

	LCD_SetCursor(Xpos, Ypos);

	if(Direction == 0)
	{
		// Prepare to write GRAM 
		LCD_WriteRAM_Prepare(); 
		for(i = 0; i < Length; i++)
		{
			LCD_WriteRAM(Color);
		}
	}
	else
	{
		for(i = 0; i < Length; i++)
		{
			// Prepare to write GRAM 
			LCD_WriteRAM_Prepare(); 
			LCD_WriteRAM(Color);
			Ypos++;
			LCD_SetCursor(Xpos, Ypos);
		}
	}
}

#define ABS(X)  ((X) > 0 ? (X) : -(X)) 

/**
 * @brief  画直线
 * @param[in] x1, y1 起点坐标
 * @param[in] x2, y2 终点坐标
 * @param[in] Color 颜色
 * @retval 无
 */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color)
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
			curpixel = 0;

	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */

	if (x2 >= x1)                 /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1)                 /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		PutPixel(x, y, Color);             /* Draw the current pixel */
		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den)             /* Check if numerator >= denominator */
		{
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}
		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}
}

/**
 * @brief  画矩形
 * @param[in] Xpos, Ypos 起点（左上角）坐标
 * @param[in] Height 高
 * @param[in] Width 宽
 * @param[in] Color 颜色
 * @retval 无
 */
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width, uint16_t Color)
{
	LCD_DrawLine(Xpos, Ypos, Width, Color, 1);
	LCD_DrawLine(Xpos, Ypos, Height, Color, 0); 
	LCD_DrawLine(Xpos, Ypos+Height, Width, Color, 1);
	LCD_DrawLine((Xpos+Width), Ypos, Height,Color, 0);
}

/**
 * @brief  画圆形
 * @param[in] Xpos, Ypos  圆心坐标
 * @param[in] Radius 半径
 * @param[in] Color 颜色
 * @retval 无
 */
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t Color)
{
	int32_t  D;/* Decision Variable */ 
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */ 

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY)
	{
		LCD_SetCursor(Xpos + CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos + CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos - CurX, Ypos + CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos - CurX, Ypos - CurY);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos + CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos + CurY, Ypos - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos - CurY, Ypos + CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		LCD_SetCursor(Xpos - CurY, Ypos - CurX);
		LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
		LCD_WriteRAM(Color);
		if (D < 0)
		{ 
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
 * @brief  清除一个区域
 * @param[in] x1, y1 左上角坐标
 * @param[in] x2, y2 右下角坐标
 * @param[in] Color 颜色
 * @retval 无
 */
void LCD_Area_Clear(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t Color)
{
	uint32_t i,j;
	for(i=0; i<y2-y1; i++)
	{  	
		LCD_SetCursor(x1, y1+i);
		LCD_WriteRAM_Prepare();
		for (j = 0; j <x2-x1; j++)
		{
			LCD_RAM_REG = Color;
		}
	}
}


#ifdef LCD_FONT

/**
 * @brief  显示一个字符
 *         font w >=8 切是8的倍数
 * @param[in] x, y 左上角坐标
 * @param[in] Font  字体信息
 * @param[in] ShowMode  显示模式
 * @retval 无
 */
void LCD_DrawFont(uint16_t x, uint16_t y, FontData *Font, DrawMode *ShowMode)
{   
	int row,col;
	int i;
	uint8_t temp;

	if (Font && Font->data && ShowMode)
	{
		for(row=0; row < Font->h; row++)
		{
			for(col=0; col < Font->w / 8; col++)
			{                 
				temp=Font->data[row * Font->w / 8 + col];
				for (i=0; i < 8; i++)
				{
					if(temp & 0x80)
					{
						//设置光标位置 
						LCD_SetCursor(x+col*8+i, y+row);
						LCD_WriteRAM_Prepare(); 
						LCD_WriteRAM(ShowMode->FColor);
					}
					else
					{
						if(ShowMode->Mode & DRAW_BACK_COLOR)
						{
							//设置光标位置 
							LCD_SetCursor(x+col*8+i, y+row);
							LCD_WriteRAM_Prepare(); 
							LCD_WriteRAM(ShowMode->BColor);
						}
					}
					temp<<=1; 
				}
			}
		}
    }
}

/**
 * @brief  显示字符串
 * @param[in] x, y 左上角坐标
 * @param[in] Str   字符串
 * @param[in] Mode 显示模式
 * @retval 无
 */
void LCD_DrawString(uint32_t x, uint32_t y, const uint8_t *Str, DrawMode *Mode)
{
	FontData Font;
	uint32_t x0;
	
	x0 = x;

	while(*Str)
	{
		if (FONT_GetDataGB2312(&Font, Str))
		{
			LCD_DrawFont(x0, y, &Font, Mode);
			x0 += Font.w;
			if (x0 >= LCD_W)
			{
				x0 = x;
				y += Font.h;
				if (y >= LCD_H)
				{
					break;
				}
			}
			Str += Font.str_bytes;
		}
		else
		{
			Str++;
		}
	}
	return ;
}

/**
 * @brief  显示字符串
 * @param[in] x, y 左上角坐标
 * @param[in] Str   字符串
 * @param[in] Mode  显示模式
 * @retval 无
 */
void LCD_DisplayNum(uint32_t x, uint32_t y, uint32_t Num, DrawMode *Mode)
{ 
	char  buf[16];
	sprintf(buf, "%d", Num);
	LCD_DrawString(x, y, (uint8_t *)buf, Mode);
}

#endif


#ifdef LCD_IMGAGE 

/**                                                                                         
 * @brief  显示一副图片                                                                     
 * @param[in] x1, y1 图片其实坐标                                                             
 * @param[in] weight, hight 图片的宽带和高度                                                            
 * @param[in] img 图片数组                                                                    
 * @retval 无                                                                               
 */                                                                                         
void LCD_Draw_Imag(uint16_t x1,uint16_t y1,uint16_t weight,uint16_t hight,const u8 *img)    
{                                                                                           
	uint32_t color,pix,i,j;                                                                   
	for(i=0; i<hight; i++)                                                                    
	{  	                                                                                      
		LCD_SetCursor(x1, y1+i);                                                                
		LCD_WriteRAM_Prepare();                                                                 
		for (j = 0; j <weight; j++)                                                             
		{                                                                                       
			color=0;                                                                              
			pix=3*(i*weight+j);                                                                   
			color=img[pix]|(img[pix+1]<<8) |(img[pix+2]<<16);                                     
			                                                                                      
			LCD_RAM_REG = color;                                                                  
		}                                                                                       
	}                                                                                         
}                                                                                           

#endif
