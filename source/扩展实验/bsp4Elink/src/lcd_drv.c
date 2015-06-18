
#include <stdio.h>
#include "delay.h"
#include "lcd.h"


/* Note: LCD /CS is NE3 - Bank 3 of NOR/SRAM Bank 1~4 */
#define LCD_BASE           ((uint32_t)(0x60000000))

#define LCD_CMD_ADDR       ((uint32_t)(0x60020000))

#define LCD_CMD_REG        (*( uint16_t *)LCD_CMD_ADDR)
#define LCD_RAM_REG        (LCD_CMD_REG)
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

	//GPIOD_OUT(3)=1;
	//GPIO_ResetBits(GPIOD, GPIO_Pin_13);


	//GPIO_ResetBits(GPIOD, GPIO_Pin_13);
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

	/*-- FSMC Configuration -------------------------*/
	/*----------------------- SRAM Bank 1 -----------*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	p.FSMC_AddressSetupTime = 5;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 9;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;
	//p.FSMC_AccessMode = FSMC_AccessMode_B;
	/* Color LCD configuration ------------------------------------
	   LCD configured as follow:
	   - Data/Address MUX = Disable
	   - Memory Type = SRAM
	   - Data Width = 16bit
	   - Write Operation = Enable
	   - Extended Mode = Enable
	   - Asynchronous Wait = Disable */

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	//FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
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
 * @brief  LCD初始化
 * @param  无
 * @retval 无
 */
void LCD_Init(void)
{ 
	/*配置LCD 用到的GPIO */
	LCD_CtrlLinesConfig();

	/* Configure the FSMC Parallel interface */
	LCD_FSMCConfig();
	//Delay_hz(50); 
	delay_ms(50);/*delay 10 ms*/

	//----Power on sequence----------
	LCD_WriteReg(0x0606,0x0000);
	delay_ms(10);
	LCD_WriteReg(0x0007,0x0001);
	delay_ms(5);
	LCD_WriteReg(0x0110,0x0001);
	delay_ms(5);
	LCD_WriteReg(0x0100,0x17B0);
	LCD_WriteReg(0x0101,0x0147);
	LCD_WriteReg(0x0102,0x0198);
	LCD_WriteReg(0x0103,0x3600);
	LCD_WriteReg(0x0281,0x0010);
	delay_ms(5);
	LCD_WriteReg(0x0102,0x01BD);
	delay_ms(5);


	LCD_WriteReg(0x0000 ,0x0000);
	//LCD_WriteReg(0x0001 ,0x0100);
	LCD_WriteReg(0x0001 ,0x0000);
	LCD_WriteReg(0x0002 ,0x0100);
	LCD_WriteReg(0x0003 ,0x1030);
	LCD_WriteReg(0x0008,0x0503);
	LCD_WriteReg(0x0009,0x0001);
	LCD_WriteReg(0x000B,0x0010);
	LCD_WriteReg(0x000C,0x0001);
	LCD_WriteReg(0x000F,0x0000);
	LCD_WriteReg(0x0007,0x0001);

	//------Panel interface control-----------
	LCD_WriteReg(0x0010,0x0015);
	LCD_WriteReg(0x0011,0x0202);
	LCD_WriteReg(0x0012,0x0300);
	LCD_WriteReg(0x0020,0x021e);
	LCD_WriteReg(0x0021,0x0202);
	LCD_WriteReg(0x0090,0x8000);
	LCD_WriteReg(0x0092,0x0000);

	//------------Voltage setting-------------
	LCD_WriteReg(0x0100,0x11b0);
	delay_ms(10);
	LCD_WriteReg(0x0101,0x0147);
	delay_ms(10);
	LCD_WriteReg(0x0102,0x018d);
	delay_ms(10);
	LCD_WriteReg(0x0103,0x1700);
	delay_ms(10);
	LCD_WriteReg(0x0107,0x0000);
	delay_ms(10);
	LCD_WriteReg(0x0110,0x0001);
	delay_ms(10);
	LCD_WriteReg(0x0210,0x0000);
	LCD_WriteReg(0x0211,0x00ef);
	LCD_WriteReg(0x0212,0x0000);
	LCD_WriteReg(0x0213,0x018f);
	LCD_WriteReg(0x0200,0x0000);
	LCD_WriteReg(0x0201,0x0000);
	LCD_WriteReg(0x0280,0x0000);
	LCD_WriteReg(0x0281,0x0000);
	LCD_WriteReg(0x0282,0x0000);
	delay_ms(10);
	//-------------Gamma setting--------------
	LCD_WriteReg(0x0300,0x1004);
	LCD_WriteReg(0x0301,0x231e);
	LCD_WriteReg(0x0302,0x2521);
	LCD_WriteReg(0x0303,0x2125);
	LCD_WriteReg(0x0304,0x1e23);
	LCD_WriteReg(0x0305,0x0410);
	LCD_WriteReg(0x0306,0x1006);
	LCD_WriteReg(0x0307,0x0610);
	LCD_WriteReg(0x0308,0x0205);
	LCD_WriteReg(0x0309,0x0104);
	LCD_WriteReg(0x030A,0x0d05);
	LCD_WriteReg(0x030B,0x0d01);
	LCD_WriteReg(0x030C,0x010d);
	LCD_WriteReg(0x030D,0x050d);
	LCD_WriteReg(0x030E,0x0401);
	LCD_WriteReg(0x030F,0x0502);

	//LCD_WriteReg(0x0400,0x3500);
	LCD_WriteReg(0x0400,0xB104);
	LCD_WriteReg(0x0401,0x0001);
	LCD_WriteReg(0x0404,0x0000);
	LCD_WriteReg(0x0500,0x0000);
	LCD_WriteReg(0x0501,0x0000);
	LCD_WriteReg(0x0502,0x0000);
	LCD_WriteReg(0x0503,0x0000);
	LCD_WriteReg(0x0504,0x0000);
	LCD_WriteReg(0x0505,0x0000);
	LCD_WriteReg(0x0600,0x0000);
	LCD_WriteReg(0x0606,0x0000);
	LCD_WriteReg(0x06F0,0x0000);

	LCD_WriteReg(0x07F0,0x5420);
	LCD_WriteReg(0x07F3,0x280f);
	LCD_WriteReg(0x07F4,0x0022);
	LCD_WriteReg(0x07F5,0x0031);//61
	LCD_WriteReg(0x07F0,0x0000);

	LCD_WriteReg(0x0007,0x0173);
	delay_ms(5);
	LCD_WriteReg(0x0007,0x0061);
	delay_ms(5);
	LCD_WriteReg(0x0007,0x0173);
	delay_ms(10);

	LCD_WriteRAM_Prepare();

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
 * @brief  根据x，y的坐标设置LCD显存位置
 * @param[in]  Xpos  x坐标
 * @param[in]  Ypos  y坐标
 * @retval 无
 */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WriteReg(LCD_REG_HADDR, Xpos);
	LCD_WriteReg(LCD_REG_VADDR, Ypos);
}


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





