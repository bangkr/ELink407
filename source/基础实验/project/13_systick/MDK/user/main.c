#include "stm32f4xx.h"
#include "uart.h"
#include <stdio.h>

#include "delay.h"

int main()
{
	int second=0;
	
	init_delay();
	
	init_uart();
	
	while(1)
	{
		
		printf(" %d second \r\n" ,second++);
		delay_ms(1000);
		
	}	
}


