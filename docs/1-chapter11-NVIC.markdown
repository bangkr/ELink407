# NVIC实验 #

STM32F40xx 总共有 92 个中断 包括 10 个内核中断和 82 个可屏蔽中断，具有 16 级可编程的中断优先级，那么这么多的中断源，可是我的COTEX M4只有一个IRQ中断信号，怎么办呢？NVIC（Nested vectored interrupt controller）可嵌套向量中断控制器，就是协助内核管理中断的使能，优先级，中断嵌套，中断挂起等待，中断触发方式等。前面的案例我们使用了外部中断（按键中断）和系统中断(systick)，那么是否系统中断要高呢？本实验我们验证一下，也是为了加深对NVIC的学习。 

## 实验原理与基础知识 ##

Cortex-M4 内核有一个 8位的寄存器支持 128 级中断嵌套，用户可以通过这个寄存器设置中断的优先级，STM32F407 只使用了这个 8 位寄存器的高四位`[7:4]`，低四位取零，这样407只使用了 16 级中断嵌套。支持 5 种优先级分组，系统复位默认使用的是优先级分组 0，也就是没有抢占式优先级，只有子优先级。

关于这个抢占优先级和这个子优先级的说明：

1. 具有高抢占式优先级的中断可以在具有低抢占式优先级的中断处理过程中被响应，即中断嵌套，或者说高抢占式优先级的中断可以嵌套低抢占式优先级的中断。 

2. 在抢占式优先级相同的情况下，有几个子优先级不同的中断同时到来，那么高子优先级的中断优先被响应。 

3  在抢占式优先级相同的情况下，如果有低副优先级中断正在执行，高副优先级的中断要等待已被响应的低副优先级中断执行结束后才能得到响应——非抢断式响应(不能嵌套)。 

4  判断中断是否被相应的依据 

 *  先是占先式优先级，其次是副优先级 
 
 *  占先式优先级决定是否会有中断嵌套 

 *  Reset、NMI、Hard Fault  优先级为负高于普通中断优先级)且不可调整 

**中断使能寄存器 NVIC_ISERx**

如果使能了挂起中断，NVIC 将激活基于此优先级的中断，如果没有使能，就是中断信号改变了挂起状态，但是 NVIC 不会激活此中断，并且忽略它的优先级。

**中断屏蔽寄存器 NVIC_ICERx**


**中断挂起寄存器 NVIC_ISPRx**


**清除中断挂起寄存器 NVIC_ICPRx**


**中断激活状态寄存器 NVIC_IABRx**


**中断优先级寄存器**


**软件触发中断寄存器 NVIC_STIR**


## 电路设计 ##

## 库函数和数据结构 ##

涉及到 NVIC 的库文件主要有 misc.c 文件以及 core_cm4.h 文件，下面就把这些相关的函数做一下解释，先介绍 misc.c 文件里面的函数，其实有了前面的介绍，这个文件里面的函数还是比较的容易理解的。

        NVIC_PriorityGroupConfig (uint32_t NVIC_PriorityGroup) 
    
这个函数用于设置优先级分组，记住系统复位默认是 0 分组，也就是没有抢占优先级，只有 0-15 的子优先级。 

        void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct) 
    
为了正确的配置优先级，之前需要先调用一下优先级分组 NVIC_PriorityGroupConfig 函数，这两个函数应该是平时用的最多的两个函数了

        void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset) 

        void NVIC_SystemLPConfig(uint8_tLowPowerMode, FunctionalState NewState) 
        
这个函数用于设置低功耗模式 


下面我们再讲一下 core_cm4.h 文件里面的函数，这个里面的函数用到的时候比较少，不过都是访问NVIC 寄存器最直接的函数，有时候用这些函数更加的方便，下面就讲解一下。 

        void NVIC_SetPriorityGrouping(uint32_t PriorityGroup) 
    
这个函数和前面的 NVIC_PriorityGroupConfig 函数是一样的，只不过一个是 ST 公司做的，一个是ARM 公司做的。

        uint32_t NVIC_GetPriorityGrouping(void) 
    
此函数用于读取优先级分组。 

        void NVIC_EnableIRQ(IRQn_Type IRQn) 
        void NVIC_DisableIRQ(IRQn_Type IRQn) 
        
这两个函数是配套使用的，注意这两个函数不能设置系统中断，系统中断有自己的函数。 

        NVIC_SetPendingIRQ(IRQn_Type IRQn) 
        NVIC_ClearPendingIRQ(IRQn_Type IRQn) 
        NVIC_GetPendingIRQ(IRQn_Type IRQn) 
    
这三个函数是一组的，设置挂起和清除挂起。 

        NVIC_GetActive(IRQn_Type IRQn) 
        
这个函数用于获得激活状态，也就是获得是否进入了相应中断程序。 

        NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) 
        uint32_t NVIC_GetPriority(IRQn_Type IRQn) 
    
这两个函数是一组的，一个用于设置中断的优先级，一个用于得到中断的优先级，它们可以设置系统中断也可以设置外设中断，前面提到我们是16级的中断优先级，所以优先级的输入范围和输出范围是 0~15。

        uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority) 
        void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority) 
        
这两个函数是一组的，一个用于优先级的编码，一个用于优先级的解码。

        void NVIC_SystemReset(void) 
    
这个函数用于实现系统的软件复位。

## 软件设计、开发 ##

本实验验证了系统中断（比如 PendSV,SVC,SysTick 等）是和外部中断（比如 USART,SPI，USB 等）是同一个优先级设置系统，不存在系统优先级就比外部中断的优先级高。这个例子就是为此而生。整体功能如下： 

按键 K2 按下：进入 K2 按键中断，如果 K2 按键中断的抢占优先级比嘀嗒定时器的抢占优先级低，那么嘀嗒定时器中断将会抢占按键中断，进入嘀嗒定时器中断后根据 K2 中断是否激活来设置变量ubPreemptionOccurred，从而来 LED 闪烁。 

按键 K3 按下：进入 K3 按键中断，K2 按键中断的抢占优先级和嘀嗒定时器的抢占优先级翻转。如果此时四个 LED 在闪烁，由于变量 ubPreemptionOccurred 被清零，LED 闪烁停止。并且根据 K2 按键中断和嘀嗒定时器谁的优先级高用串口 1 打印相关信息，方便观察现象，从而方便操作。 

下面详细的分析一下程序，这里按键只是实现按键中断这一个作用，进入中断设置相应函数， 然后退出中断，不做按键的识别。所以这个程序重点的说一下按键中断函数和主函数即可。

*1 按键中断函数* 

	void EXTI3_IRQHandler(void) 
	{ 
		if(EXTI_GetITStatus(EXTI_Line3) != RESET) 
		{ 
			EXTI_ClearITPendingBit(EXTI_Line3); /* 清除中断标志位 */ 
		} 
	
		/* 按键K2按下 */ 
		if(EXTI_GetITStatus(EXTI_Line3) != RESET) 
		{ 
			/* 这里简单的做一个延迟，防止按键抖动造成多次进入中断 */ 
			Delay(10000000); 
			/* 产生嘀嗒定时器中断 */ 
			SCB->ICSR |= 0x04000000; 
			EXTI_ClearITPendingBit(EXTI_Line8); /* 清除中断标志位 */ 
			printf("K2按键按下\r\n"); 
		} 
	} 
	void EXTI2_IRQHandler(void) 
	{ 
		NVIC_InitTypeDef NVIC_InitStructure; 
		if(EXTI_GetITStatus(EXTI_Line11) != RESET) 
		{ 
			EXTI_ClearITPendingBit(EXTI_Line11); /* 清除中断标志位 */ 
		} 
		/* 按键K3按下 */ 
		if(EXTI_GetITStatus(EXTI_Line13) != RESET) 
		{ 
			ubPreemptionPriorityValue = !ubPreemptionPriorityValue; 
			ubPreemptionOccurred = 0; 
			/* 这里简单的做一个延迟，防止按键抖动造成多次进入中断 */ 
			Delay(10000000); 
			/* 修改K1按键中断的优先级 */ 
			NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; 
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ubPreemptionPriorityValue; 
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
			NVIC_Init(&NVIC_InitStructure); 
			/* 配置嘀嗒定时器的抢占优先级和子优先级 */ 
			NVIC_SetPriority(SysTick_IRQn, 
					NVIC_EncodePriority(NVIC_GetPriorityGrouping(), !ubPreemptionPriorityValue, 0)); 
			EXTI_ClearITPendingBit(EXTI_Line2); /* 清除中断标志位 */ 
			printf("K3按键按下，嘀嗒定时器和K3按键的抢占优先级都翻转\r\n"); 
			if(ubPreemptionPriorityValue == 0) 
			{ 
				printf("滴答定时器的中断不可以抢占K2按键中断，按下K2按键LED不闪烁\r\n"); 
			} 
			else 
			{ 
				printf("滴答定时器的中断可以抢占K2按键中断，按下K2按键LED闪烁\r\n"); 
			} 
		} 
		if(EXTI_GetITStatus(EXTI_Line15) != RESET) 
		{ 
			EXTI_ClearITPendingBit(EXTI_Line15); /* 清除中断标志位 */ 
		} 
	} 

这里为了说明问题，在中断函数里面加了一个延迟函数，用于滤波，防止按键的抖动造成多次进入中断，实际项目中切勿这么做，严重影响系统性能。同时为了方便大家观察现象，K3 按键按下会有相应的串口信息打印，从而方便大家操作。 在 K1 按键中断中，通过 SCB->ICSR |= 0x04000000;设置嘀嗒定时器中断挂起，如果滴答定时器的抢占优先级比 K2 按键中断的优先级高将发生中断嵌套。下面是嘀嗒定时器中断服务程序：

	void SysTick_Handler(void) 
	{ 
		/* 如果按键K2中断被嘀嗒定时器抢占 */ 
		if(NVIC_GetActive(EXTI3_IRQn) != 0) 
		{ 
			ubPreemptionOccurred = 1; 
		} 
	} 

通过函数 NVIC_GetActive 来查询 K2 按键释放激活。 

*2.  主函数*
 
	int main(void)
	{
	
		init_uart(); 	/* 初始化串口 */
		init_button();
		EXTIX_Init();
		init_delay();
		/* 针对不同的应用程序，添加需要的底层驱动模块初始化函数 */
	
		init_led(); 		/* 初始LED指示灯端口 */
	
	
		/* 配置嘀嗒定时器的抢占优先级和子优先级 */
		NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), !ubPreemptionPriorityValue, 0));
		/* 进入主程序循环体 */
		while (1)
		{
			if(ubPreemptionOccurred != 0)
			{
				/* 翻转 LED1 */
				Led_toggle(1);
				Delay(0x5FFFF);
	
				/* 翻转 LED2 */
				Led_toggle(2);
				Delay(0x5FFFF);
	
			}
		}
	}
	
## 软件下载、测试验证 ##

