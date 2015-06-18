#include "led.h"

void init_led(void)
{
	
	
	RCC->AHB1ENR|=1<<0;									//使能PORTA时钟 
	RCC->AHB1ENR|=1<<1;									//使能PORTB时钟 
	
	GPIOA_Reg->	MODER&=~(3<<(8*2));  		//先清除 PA8 原来的设置
	GPIOA_Reg->	MODER|=1<<(8*2);    		//设置 PA8 为输出模式
	GPIOA_Reg->	OTYPER&=~(1<<8) ;    		//清除 PA8 原来的设置
	GPIOA_Reg->	OTYPER|=0<<8;      			//设置 PA8 为推挽输出
	GPIOA_Reg-> OSPEEDR&=~(3<<(8*2)); 	//先清除 PA8 原来的设置
	GPIOA_Reg-> OSPEEDR|=3<<(8*2);    	//设置 PA8 输出速度为 100Mhz 
	GPIOA_Reg-> PUPDR&=~(3<<(8*2));  		//先清除 PA8 原来的设置
	GPIOA_Reg-> PUPDR|=0<<(8*2);    		//设置 PA8 不带上下拉
	GPIOA_Reg->	ODR|=1<<8;        			//设置 PA8 输出 1（高电平,LED1初始为关闭状态）
	
	GPIOB_Reg->	MODER&=~(3<<(8*2));  		//先清除 PB8 原来的设置
	GPIOB_Reg->	MODER|=1<<(8*2);    		//设置 PB8 为输出模式
	GPIOB_Reg->	OTYPER&=~(1<<8) ;    		//清除 PB8 原来的设置
	GPIOB_Reg->	OTYPER|=0<<8;      			//设置 PB8 为推挽输出
	GPIOB_Reg-> OSPEEDR&=~(3<<(8*2)); 	//先清除 PB8 原来的设置
	GPIOB_Reg-> OSPEEDR|=3<<(8*2);    	//设置 PB8 输出速度为 100Mhz 
	GPIOB_Reg-> PUPDR&=~(3<<(8*2));  		//先清除 PB8 原来的设置
	GPIOB_Reg-> PUPDR|=0<<(8*2);    		//设置 PB8 不带上下拉
	GPIOB_Reg->	ODR|=1<<8;        			//设置 PB8 输出 1（高电平,LED1初始为关闭状态）
	
	
}


void led_on(int ledx)
{
#ifdef	BIT_BAND_ENB	
	if(LED1==ledx)
	{
		PAout(8)=0;        								//设置 PA8 输出 0（低电平,LED1亮）
	
	}
	else if(LED2==ledx)
	{
		PBout(8)=0;;        							//设置 PB8 输出 0（低电平,LED2亮）
		
	}
	else
	{
		;																	//ledx 不存在处理
	}
	
#else
	
	if(LED1==ledx)
	{
		GPIOA_Reg->	ODR &=~(1<<8);        //设置 PA8 输出 0（低电平,LED1亮）
	
	}
	else if(LED2==ledx)
	{
		GPIOB_Reg->	ODR &=~(1<<8);        //设置 PB8 输出 0（低电平,LED2亮）
		
	}
	else
	{
		;																	//ledx 不存在处理
	}
	

#endif
}

void led_off(int ledx)
{
#ifdef	BIT_BAND_ENB	
	
	if(LED1==ledx)
	{
		PAout(8)=1;        								//设置 PA8 输出 1（高电平,LED1灭）
	
	}
	else if(LED2==ledx)
	{
		PBout(8)=1;        								//设置 PB8 输出 1（高电平,LED2灭）
		
	}
	else
	{
		;																	//ledx 不存在处理
	}
	
	#else
	
	if(LED1==ledx)
	{
		GPIOA_Reg->	ODR|=1<<8;        		//设置 PA8 输出 1（高电平,LED1灭）
	
	}
	else if(LED2==ledx)
	{
		GPIOB_Reg->	ODR|=1<<8;        		//设置 PB8 输出 1（高电平,LED2灭）
		
	}
	else
	{
		;																	//ledx 不存在处理
	}
	
	
#endif
	
}   
