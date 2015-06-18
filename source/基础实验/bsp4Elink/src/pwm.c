#include "pwm.h"

//arr：自动重装值  psc：时钟预分频数 
void timer_pwm_init(u32 arr,u32 psc) 
{                   
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure; 
	TIM_OCInitTypeDef    TIM_OCInitStructure; 
   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //TIM4 时钟使能      
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能PB时钟 
   
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4); //PB8复用为TIM4 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                       //GPIOB8   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //复用功能 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  //速度50MHz 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            //推挽复用输出 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //上拉 
	GPIO_Init(GPIOB,&GPIO_InitStructure);                            //初始化PB8 
     
	TIM_TimeBaseStructure.TIM_Prescaler=psc;    //定时器分频 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
	TIM_TimeBaseStructure.TIM_Period=arr;      //自动重装载值 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器4
   
	//初始化TIM4 Channel3 PWM模式    
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM调制模式1 
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);    //初始化外设TIM4
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);    //使能预装载寄存器 
    TIM_ARRPreloadConfig(TIM4,ENABLE);//ARPE使能   
	TIM_Cmd(TIM4, ENABLE);    //使能TIM4    

	//高级定时器，使能主输出，普通定时器不用加
	TIM_CtrlPWMOutputs(TIM4, ENABLE); 
}      
