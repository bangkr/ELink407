#include "delay.h"

__IO u16 ntime;								    

void delay_ms(u16 nms)
{	 		  	  
	delay_us(nms*1000);	  	    
}   
		    								   
void delay_us(u32 nus)
{		
  	u32 count=0;
  	u32 utime=(120*nus/7);
    while(1)if(++count>utime)break;
}

void DDelay(u32 count)
{
	while(count--);
}

