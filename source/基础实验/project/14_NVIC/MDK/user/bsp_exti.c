#include "stm32f4xx.h"

//初始化PD6，PE2~4为中断输入
void EXTIX_Init(void)
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	EXTI_InitTypeDef    EXTI_InitStructure;
	
	//init_button();//按键对应的 IO 口初始化
	
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



void bsp_InitKeyEXTI(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* 使能SYSCFG时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* 连接 EXTI Line8 到 PI8 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource8);
	/* 连接 EXTI Line13 到 PC8 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	/* 连接 EXTI Line11 到 PI11 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource11);
	/* 连接 EXTI Line3 到 PH3 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	/* 连接 EXTI Line15 到 PH15 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource15);
	/* 连接 EXTI Line7 到 PG7 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource7);
	/* 连接 EXTI Line2 到 PH2 引脚 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	/* 连接 EXTI Line11 到 PF11 引脚 */
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource11);
	
	/* 配置 EXTI LineXXX */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line3 | EXTI_Line7 | EXTI_Line8 \
	                              |EXTI_Line11| EXTI_Line13| EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	
 	/* 设置NVIC优先级分组为Group1：0-1抢占式优先级，0-7的响应式优先级 */
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	/* 中断优先级配置 最低优先级 这里一定要分开的设置中断，不能够合并到一个里面设置 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* K1按键 PI8*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* K2按键 PC13*/
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
