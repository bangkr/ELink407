#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"

#include "rng.h"


int main()
{
	int button_value=0;
	
	init_led();
	
	init_delay();
	
	init_uart();
	
	init_button();
	
	if(init_rng())
	{
		printf("RNG error\r\n");
	}
	
	while(1)
	{
						
		button_value=button_read();
	
		if(button_value>0)
		{
			mydelay(100);		//延时去抖
		
			if(button_value==button_read())
			{
					switch(button_value)
					{
						case 1:
							led_on(LED1);
							printf("random number %u\r\n", get_RandomNum());				
							break;
					
						case 2:
							led_on(LED2);
							printf("random number[0-255] %d\r\n", get_RandomRange(0,0xFF));	
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
	
		mydelay(100);
		
	}

	
	
}


