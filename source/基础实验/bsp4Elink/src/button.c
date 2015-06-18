#include "button.h"
#include "button_noexti.h"
#include "led.h"
#include "delay.h"
#include "stm32f4xx.h"

//初始化PD6，PE2~4为中断输入
void EXTIX_Init(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	EXTI_InitTypeDef    EXTI_InitStructure;
	
	init_button();//按键对应的 IO 口初始化
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);			 //使能 SYSCFG 时钟
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);	//PE2 连接线 2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);	//PE3 连接线 3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);	//PE4 连接线 4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);	//PD6 连接线 6
	
	/*  配置 EXTI_Line6 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;						//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 			//上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//使能 LINE0
	EXTI_Init(&EXTI_InitStructure);

	/*  配置 EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   		//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//中断线使能
	EXTI_Init(&EXTI_InitStructure);									//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//外部中断 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0x01;		//抢占优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);									//配置 NVIC
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;				//外部中断 2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);									//配置 NVIC
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;				//外部中断 3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//响应优先级 2 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);									//配置 NVIC
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;				//外部中断 4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;	//抢占优先级 4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);									//配置  NVIC 
	
}


//PIN6外部中断 9_5 服务程序
void EXTI9_5_IRQHandler(void)
{ 
	mydelay(10); //消抖
	if(1==button_read()) 
	{ 
		led_on(LED1); //LED1 亮
	} 
	EXTI_ClearITPendingBit(EXTI_Line6); //清除 LINE0 上的中断标志位 
}

//外部中断 2 服务程序
void EXTI2_IRQHandler(void)
{ 
	mydelay(10); //消抖
	if(2==button_read()) 
	{  
		led_on(LED2); //LED2 亮
	} 
	EXTI_ClearITPendingBit(EXTI_Line2);//清除 LINE2 上的中断标志位 
}

//外部中断 3 服务程序
void EXTI3_IRQHandler(void)
{ 
	mydelay(10); //消抖
	if(4==button_read()) 
	{ 
		led_on(LED1);		//LED1 亮
		led_on(LED2);		//LED2 亮
	} 
	EXTI_ClearITPendingBit(EXTI_Line3);   //清除 LINE3 上的中断标志位 
}

//外部中断 4 服务程序
void EXTI4_IRQHandler(void)
{
	mydelay(10); //消抖
	
	if(8==button_read()) 
	{  
		led_off(LED1);		//LED1 灭
		led_off(LED2);		//LED2 灭
	} 
	EXTI_ClearITPendingBit(EXTI_Line4);//清除 LINE4 上的中断标志位 
}  

