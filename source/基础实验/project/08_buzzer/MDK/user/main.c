#include "buzzer.h"

void mydelay(int delaytime);

int main(void)
{
	init_buzzer();
	
	while(1)
	{
		buzzer_on();
		
		
		mydelay(1000);
		
		buzzer_off();
	
		
		mydelay(1000);
	}
}

void mydelay(int delaytime)
{
	int i;
	
	for(;delaytime>0;delaytime--)
				for(i=0;i<5000;i++)
						;
		
}  

