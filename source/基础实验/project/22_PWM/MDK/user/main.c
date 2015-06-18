#include "stm32f4xx.h"
#include "uart.h"
#include "pwm.h"
#include "delay.h"

int main()
{
	u16 led0pwmval=0;       
	u8 dir=1; 

	init_uart();
	init_delay();
	
	timer_pwm_init(500-1,84-1);  //定时器1时钟为168M，分频系数为168，所以计数频率 
	//为168M/168=1Mhz,重装载值500，所以PWM频率为:1M/500=2Khz,脉宽周期为：1/2000=0.5ms
	while(1)    
	{ 
		delay_ms(10);    
		if(dir)
			led0pwmval++;//dir==1 led0pwmval 递增 
		else 
			led0pwmval--; //dir==0 led0pwmval递减   
		if(led0pwmval>300){
			dir=0;//led0pwmval 到达300后，方向为递减 
		}
		if(led0pwmval==0){
			dir=1;  //led0pwmval 递减到0后，方向改为递增 
		}
		TIM_SetCompare3(TIM4,led0pwmval);  //修改比较值，修改占空比 
	} 
}
