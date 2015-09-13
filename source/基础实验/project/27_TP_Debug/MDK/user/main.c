
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "uart.h"
#include "IIC.h"
#include "tp.h"
#include "tslib.h"
#include "delay.h"

#include "lcd.h"

#include <stdio.h>
#include <string.h>

/**
  * @brief  Main program
  */

extern volatile unsigned int pen_flag;

uint8_t WriteBuffer[256]={0};
uint8_t ReadBuffer[256]={0};  


int main(void)
{
	init_uart();
   
	printf("\r\n***************************************\r\n");
	
	I2C_Configuration();
	
	TpInit(); 
	
	init_delay();

	init_lcd();	

	LCD_Clear(0x11);
		
	ts_test();

	//	printf("\r\n ZT2083  Read Test END \r\n");	

}

/*****END OF FILE****/
