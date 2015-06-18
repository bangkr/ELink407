    #include "wakeup.h"
    #include "led.h"
	#include "stm32f4xx.h"
    #include "delay.h"
	#include "stdio.h"
   
     
 //进入待机模式
 void StandbyMode(void)
 {	
	//关闭不需要的外设
		
	RCC_AHB1PeriphResetCmd(0X01FF,ENABLE);  //复位所有 IO 口
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能 PWR 时钟
	PWR_ClearFlag(PWR_FLAG_WU);//清除 Wake-up  标志
	PWR_WakeUpPinCmd(ENABLE);//设置 WKUP 用于唤醒
	PWR_EnterSTANDBYMode();//进入待机模式
		
 }
	
	
u8 Check_WKUP(void)
{
	u32 t=0;
		
	led_on(LED1); //亮灯LED1
		
	while(1)
	{
		if(0==(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)&0x01))//wakeup键按下
		{
			t++;
		}

		if(t>=600000)//按下延时
		{
			printf("待机\r\n");
			return 2; 
		}
	}
}
	
	
    //中断,检测到PA0脚的一个上升沿.
    //中断线0线上的中断检测
    void EXTI0_IRQHandler(void)
    {
		EXTI->PR=1<<0; //清除LINE10上的中断标志位
		if(2==Check_WKUP())//待机?
		{	
			StandbyMode();
		}
    }
    
	
//PA0 WKUP唤醒初始化
void init_wkup(void)
{	
		
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	EXTI_InitTypeDef    EXTI_InitStructure;
	
	NVIC_InitTypeDef    NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能 GPIOA 时钟 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能 SYSCFG 时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉 
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能 LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 连接到线 0

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置 NVIC
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组 2
}

