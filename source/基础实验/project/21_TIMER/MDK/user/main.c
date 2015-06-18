#include "stm32f4xx.h"
#include "uart.h"
#include <stdio.h>
#include "timer.h"


int main()
{
	float f=1.2334;
	init_uart();
	printf(" test!\r\n");
	printf(" f=%f\r\n",f);
	//Tout=((arr+1)*(psc+1)),
	timer_init(10000-1,8400-1) ;//84M等于APB1定时器时钟主频，即每秒产生一次定时器中断 
	while(1);
}
