#include "stm32f4xx.h"
#include "delay.h"
#include "lcd.h"

int main(void)
{

	init_delay();
	
	init_lcd();	

	while(1)
	{
		LCD_Clear(Red);
		delay_ms(1000);

		LCD_Clear(White);
		delay_ms(1000);

	}
}

