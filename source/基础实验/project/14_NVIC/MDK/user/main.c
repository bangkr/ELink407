#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "button.h"

#include "button_noexti.h"

int main()
{
	int button_value=0;
	
	init_led();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组 2
	 
	EXTIX_Init();       //初始化外部中断输入
	
	
		while(1)
		{
		
#if 0
		button_value=button_read();
	
		if(button_value>0)
		{
			mydelay(10);		//延时去抖
		
			if(button_value==button_read())
			{
					switch(button_value)
					{
						case 1:
							led_on(LED1);
							break;
					
						case 2:
							led_on(LED2);
							break;
					
						case 4:
							led_on(LED1);
							led_on(LED2);
							break;
					
						case 8:
							led_off(LED1);
							led_off(LED2);
							break;
					
						default:			//两个键同时按下，不处理
							break;
					}
				}
		}
	#endif
		mydelay(1000);
		
	}

	
	
}


