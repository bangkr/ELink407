#include "stm32f4xx.h"
#include "button_noexti.h"

void init_button(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_BUTTON Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Configure the GPIO_BUTTON pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 |GPIO_Pin_4 ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}


int button_read(void)
{
	int status=0;
	
	status=  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)<<3
				 | GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)<<2
				 | GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)<<1
				 | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)<<0;
	
	return (~status) &0xF;
		
}
