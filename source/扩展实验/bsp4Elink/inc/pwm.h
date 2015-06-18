#ifndef __PWM_H_
#define __PWM_H_

#include "stm32f4xx.h"

//arr：自动重装值  psc：时钟预分频数 
//TIM14_PWM_Init(500-1,84-1);  //定时器时钟为168M，分频系数为sc，所以计数频率 
//为168/psc,重装载值arr，所以PWM频率为168/(psc+1)/(arr+1)  

void timer_pwm_init(u32 arr,u32 psc);


#endif
