#include "stm32f4xx.h"
#include "stdio.h"

#include "uart.h"


#define STRLEN 300

int main()
{

	char helpstr[STRLEN]=" \
------------------------------\r\n \
|                            |\r\n \
|      helloworld .v1.0      |\r\n \
|      'h'  --> 'hello'      |\r\n \
|      'w'  --> 'world'      |\r\n \
|                            |\r\n \
------------------------------\r\n" ;
	
	uint8_t cmdchar;  /* 按键代码 */ 
	
	
	
	init_uart();   /* 串口初始化 */ 
	
	//printf("%s",helpstr);
		
	/* 使能 DMA clock */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	
	/* 使能 USART DMA TX 请求 */ 
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 
	

	USART_DMA_TxConfig((uint32_t *)helpstr, STRLEN); 
   /* 等待数据传输完成，这个函数可以不是用的，直接用下面的DMA等待即可 */ 
   
	while(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7)==RESET); 
   /* 清除DMA传输完成标志 */ 
   DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7); 
   /* 清除串口传输完成标志 */ 
   USART_ClearFlag(USART1,USART_FLAG_TC); 	
		
	while(1)
	{
		
		 
		printf("#");
		scanf("%c",&cmdchar);
		
		if(cmdchar)
		{
		
			switch(cmdchar)
			{
				case 'h':
					printf("hello \r\n");
					break;
			
				case 'w':
					printf("world \r\n");
					break;
			
				default:
					printf("command error \r\n");
					printf("%s",helpstr);
					break;
			}
		}
	}
	
}
