#include "led.h"
#include "stm32f4xx.h"


void init_led(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);//关闭LED1 
	GPIO_SetBits(GPIOB,GPIO_Pin_8);//关闭LED2
	
}

void led_on(int ledx)
{
	if(LED1==ledx)
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);//点亮LED1 
	if(LED2==ledx)
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);//点亮LED2
}

void led_off(int ledx)
{
	if(LED1==ledx)
		GPIO_SetBits(GPIOA,GPIO_Pin_8);//关闭LED1 
	if(LED2==ledx)
		GPIO_SetBits(GPIOB,GPIO_Pin_8);//关闭LED2
	
}

void Led_toggle(unsigned char _no)
{
	if (_no == LED1)
	{
		GPIO_ToggleBits(GPIOA,GPIO_Pin_8);
	}
	else if (_no == LED2)
	{
		GPIO_ToggleBits(GPIOB,GPIO_Pin_8);
	}
}

