#include "led.h"

void mydelay(int delaytime);

int main(void)
{
	init_led();
	
	while(1)
	{
		led_on(LED1);
		led_off(LED2);
		
		mydelay(1000);
		
		led_off(LED1);
		led_on(LED2);
		
		mydelay(1000);
	}
}


void mydelay(int delaytime)
{
	int i;
	
	for(;delaytime>0;delaytime--)
				for(i=0;i<1000;i++)
						;
		
}  
