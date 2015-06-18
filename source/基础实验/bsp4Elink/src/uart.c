#include "stm32f4xx.h"
#include "stdio.h"

void init_uart(void) 
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure; 
	 
	/* 打开 GPIO 时钟 */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* 打开 UART 时钟 */ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	/* 将 PA9 映射为 USART1_TX */ 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); 
	/* 将 PA10 映射为 USART1_RX */ 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); 
	
	/* 配置 USART Tx 为复用功能 */ 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; /* 输出类型为推挽 */ 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  /* 内部上拉电阻使能 */ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  /* 复用模式 */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/* 配置 USART Rx 为复用功能 */ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/*  配置串口硬件参数 */ 
	USART_InitStructure.USART_BaudRate = 115200;  /* 波特率 */ 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No ; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	
		
	USART_Cmd(USART1, ENABLE);   /* 使能串口 */ 

	USART_ClearFlag(USART1, USART_FLAG_TC); /* 清发送完成标志 */ 
} 


int fputc(int ch, FILE *f) 
{ 
	/* 写一个字节到USART1 */ 
	USART_SendData(USART1, (uint8_t) ch); 
	
	/* 等待发送结束 */ 
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) 
	{
	} 
	return ch; 
} 


int fgetc(FILE *f) 
{ 
	/* 等待串口1输入数据 */ 
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); 
	return (int)USART_ReceiveData(USART1); 
}

//#define UART_DMA

#ifdef UART_DMA

#define TIMEOUT_MAX 1000 /* 定义最大的溢出数值 */

void USART_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize) 
{ 
	DMA_InitTypeDef DMA_InitStructure; 
	__IO uint32_t Timeout = TIMEOUT_MAX; 
	/* 复位 DMA Stream 寄存器 (用于调试目的) */ 
	/* DMA_DeInit(USARTx_TX_DMA_STREAM); */ 
	/* 
	使能前检测DMA是否被禁止。   请注意，在使用相同数据流的多次传输时，这一步很有用：使能，然后禁止，然后使  能..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，在确保使能 
	位被硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。 
	*/ 
	DMA_Cmd(DMA2_Stream7, DISABLE); 
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE) 
	{ 
	} 
	DMA_InitStructure.DMA_BufferSize = BufferSize; 
	/* 配置DMA大小 */ 
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; 
	/* 在这个程序里面使能或者禁止都可以的 */ 
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	/* 设置阀值 */ 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ; 
	/* 设置内存为单字节突发模式 */ 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	/* 设置内存数据的位宽是字节 */ 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	/* 使能地址自增 */ 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 

	/* 设置DMA是正常模式 */ 
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR)) ; 
	/* 设置外设地址 */ 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
	/* 设置外设为单字节突发模式 */ 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;; 
	/* 设置外设数据的位宽是字节 */ 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	/* 禁止外设地址自增 */ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
	/* 设置优先级 */ 
	/* 配置 TX DMA */ 
	DMA_InitStructure.DMA_Channel = DMA_Channel_4 ; 
	/* 配置发送通道 */ 
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral ; 
	/* 设置从内存到外设 */ 
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BufferSRC ; 
	/* 设置内存地址 */ 
	DMA_Init(DMA2_Stream7,&DMA_InitStructure); 
	/* 使能 DMA USART TX Stream */ 
	DMA_Cmd(DMA2_Stream7, ENABLE); 
	/* 
	检测DMA Stream 是否被正确的使能.如果DMA的参数配置错误了，那么DMA Stream   的使能位会被硬件清除掉，从而使得传输停止比如FIFO的阀值配置错误   */ 
	Timeout = TIMEOUT_MAX; 
	while ((DMA_GetCmdStatus(DMA2_Stream7) != ENABLE) && (Timeout-- > 0)) 
	{ 
	} 
	/* 检测是否超时 */ 
	if (Timeout == 0) 
	{	 
		/* 处理错误 */ 
		//TimeOut_UserCallback(); 
	} 
} 


#if 0 
void USART_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize) 
{ 
	DMA_InitTypeDef DMA_InitStructure; 
	__IO uint32_t Timeout = TIMEOUT_MAX; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2 时钟使能
	
	/* 
	使能前检测DMA是否被禁止。   请注意，在使用相同数据流的多次传输时，这一步很有用：使能，然后禁止，然后使  能..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，在确保使 
	能位被硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。 
	*/ 
	DMA_Cmd(DMA2_Stream7, DISABLE); 
	while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE) 
	{	
		
	} 
	DMA_InitStructure.DMA_BufferSize = BufferSize; 
	/* 配置DMA大小 */ 
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; 

	/* 在这个程序里面使能或者禁止都可以的 */ 
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	/* 设置阀值 */ 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ; 
	/* 设置内存为单字节突发模式 */ 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
	/* 设置内存数据的位宽是字节 */ 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	/* 使能地址自增 */ 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
	/* 设置DMA是正常模式 */ 
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t) (&(USART1->DR)) ; 
	/* 设置外设地址 */ 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
	/* 设置外设为单字节突发模式 */ 
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
	/* 设置外设数据的位宽是字节 */ 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	/* 禁止外设地址自增 */ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; 
	/* 设置优先级 */ 
	/* 配置 RX DMA */ 
	DMA_InitStructure.DMA_Channel = DMA_Channel_4 ; 
	/* 配置接收通道 */ 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ; 
	/* 设置从外设到内存 */ 
	DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)BufferDST ; 
	/* 设置内存地址 */ 
	DMA_Init(DMA2_Stream7,&DMA_InitStructure); 
	/* 使能 DMA USART TX Stream */ 
	DMA_Cmd(DMA2_Stream7, ENABLE); 
	/* 
	检测DMA Stream 是否被正确的使能.如果DMA的参数配置错误了，那么DMA Stream   的使能位会被硬件清除掉，从而使得传输停止比如FIFO的阀值配置错误   */ 
	Timeout = TIMEOUT_MAX; 
	while ((DMA_GetCmdStatus(DMA2_Stream7) != ENABLE) && (Timeout-- > 0)) 
	{ 
	} 
	/* 检测是否超时 */ 
	if (Timeout == 0) 
	{ 
	/* 处理错误 */ 
	;//TimeOut_UserCallback(); 
	} 

	NVIC_InitTypeDef NVIC_InitStructure;

	/* 使能发送传输完成中断 */ 
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); 
	/* 使能接收传输完成中断 */ 
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); 
	/* 使能 DMA Stream 中断通道 */ 
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	/* 使能 DMA Stream 中断通道 */ 
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
}
#endif 

//DMA 的中断服务程序 

void DMA2_Stream7_IRQHandler(void) 
{ 
 /* 
 函数DMA_GetITStatus在使用上存在问题，产生标志了，检测无法通过。但是使用函  数DMA_GetFlagStatus却可以，这里记录一下这个问题，方便后面解决。 
 */ 
  if (DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)==SET) 
//  if (DMA_GetITStatus(USARTx_TX_DMA_STREAM,USARTx_TX_DMA_FLAG_TCIF) == SET) 
  { 
   /* 清除DMA传输完成标志 */  //   
	  DMA_ClearITPendingBit(DMA2_Stream7, DMA_FLAG_TCIF7);   
  } 
 /* 这里采用寄存器直接操作 */ 
// if(DMA2->HISR & USARTx_TX_DMA_FLAG_TCIF) 
//{ 
//  DMA2->HIFCR = USARTx_TX_DMA_FLAG_TCIF; 
//} 
} 

void USARTx_DMA_RX_IRQHandler(void) 
{ 
 /* 这里采用寄存器直接操作 */ 
 if(DMA2->LISR & DMA_FLAG_TCIF7) 
{ 
  DMA2->LIFCR = DMA_FLAG_TCIF7; 
  /* 模拟一个按键按下 */ 
  //bsp_PutKey(KEY_4_DOWN); 
} 
} 


#endif

