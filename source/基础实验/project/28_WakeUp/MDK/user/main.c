#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"

#include "wakeup.h"


int main()
{
	init_led();
	
	init_delay();
	
	init_uart();
	
	printf("开机唤醒\r\n");
	
	init_wkup();
	
	while(1)
	{
		
		led_on(LED2);
		
		delay_ms(1000);
		
		led_off(LED2); 
		
		delay_ms(1000);	
		
		printf("主程序while 循环\r\n");
		
	}
	
}


