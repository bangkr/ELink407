#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"


int main()
{
	int i;
	int button_value=0;
	
	int UID_baseaddr=0x1FFF7A10;
	
	unsigned int CPU_UID[3];
	
	init_led();
	
	init_delay();
	
	init_uart();
	
	init_button();
	
	for(i=0;i<3;i++)
	{
		CPU_UID[i]=*((vu32*)UID_baseaddr +i);
		printf("CPU_UID[%d] =%d\r\n",i,CPU_UID[i]);
	}
	
	

	
	while(1)
	{
		
				
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
	
		mydelay(1000);
		
	}

	
	
}


