#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"

void mydelay(int delaytime);

int main()
{
	int button_value=0;
	
	init_led();
	init_button();
	
	while(1)
	{
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
	
	}
	
}

void mydelay(int delaytime)
{
	int i;
	
	for(;delaytime>0;delaytime--)
				for(i=0;i<1000;i++)
						;
		
}  

