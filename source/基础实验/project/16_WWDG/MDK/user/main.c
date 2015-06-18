#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"
#include "iwdg.h"

#include "wwdg.h"

//模拟一段非正常指令段
void error_code()
{
	printf("模拟进入非正常代码段,延时3s不喂狗，观察看门狗复位\r\n");
	mydelay(3000);  

}

int main()
{
	int button_value=0;
	
	init_led();
	
	init_delay();
	
	init_uart();
	
	init_button();
	
	printf("系统复位，开启独立看门狗\r\n");
	
	init_iwdg(4,1500); //与分频数为 64,重载值为 500,溢出时间为 1s
	
	init_wwdg(0x7F,0X5F,WWDG_Prescaler_8);
	
	while(1)
	{
		
		feed_iwdg();
		
		button_value=button_read();
	
		if(button_value>0)
		{
			mydelay(1);		//延时去抖
		
			if(button_value==button_read())
			{
					switch(button_value)
					{
						case 1:
							led_on(LED1);
							printf("k1 被按下\r\n");
							error_code();
							break;
					
						case 2:
							led_on(LED2);
							printf("k2 被按下,关闭窗口看门狗中断\r\n");
							wwdg_IRQ(DISABLE);
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
	
		mydelay(1000);
		
	}

	
	
}


