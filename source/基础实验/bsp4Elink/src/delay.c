#include "delay.h"
#include "stm32f4xx.h"

unsigned int TimingDelay;

void init_delay(void)
{
	/* 
	设置嘀嗒定时器是1us中断： 
	函数SysTick_Config()是属于CMSIS里面的一个函数，实现配置如下 
		- 函数的参数是重载寄存器reload要赋予的初值
  	- 配置嘀嗒定时器的优先级是最低优先级15
  	- 配置HCLK作为系统的时钟源
  	- 使能嘀嗒定时器中断
  	- 启动嘀嗒定时器计数 
	*/ 
	if (SysTick_Config(SystemCoreClock / 1000000)) 
	{ 
		/* 出错 */ 
		while (1); 
	} 
}

void TimingDelay_Decrement(void) 
{ 
	if (TimingDelay != 0) 
	{ 
		TimingDelay--; 
	} 
} 

void delay_ms( unsigned int mTime) 
{ 
	TimingDelay = mTime*1000; 
	while(TimingDelay != 0); 
}

void delay_us(unsigned int uTime)
{		
	TimingDelay = uTime; 
	while(TimingDelay != 0); 	 
}


void mydelay(int delaytime)
{
	delay_ms(delaytime);
	
#if 0     //使用标准延时函数
	int i;
	
	for(;delaytime>0;delaytime--)
				for(i=0;i<1000;i++)
#endif
						;		
}  
