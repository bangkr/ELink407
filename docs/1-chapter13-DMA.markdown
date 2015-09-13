# DMA实验 #

本章我们将向大家介绍 STM32F4 的 DMA。在本章中，我们将利用 STM32F4 的 DMA 来实现UART数据的发送，以此学习DMA的使用。

## 实验原理与基础知识 ##

Direct Memory Access(DMA)，即直接存储器访问。DMA 传输方式无需 CPU 直接控制传输，也没有中断处理方式那样保留现场和恢复现场的过程，通过硬件为 RAM 与 I/O 设备开辟一条直接传送数据的通路，能使 CPU 的效率大为提高。

STM32F4 最多有 2 个 DMA 控制器（DMA1 和 DMA2） ，共 16 个数据流（每个控制器 8 个），每一个 DMA 控制器都用于管理一个或多个外设的存储器访问请求。每个数据流总共可以有多达 8个通道（或称请求）。每个数据流通道都有一个仲裁器，用于处理 DMA 请求间的优先级。 

DMA 的框图

![](img/chapter13/1.1.1.png) 

DMA 控制器执行直接存储器传输：因为采用 AHB 主总线，它可以控制 AHB 总线矩阵来启动 AHB 事务。它可以执行下列事务：

1，外设到存储器的传输

2，存储器到外设的传输

3，存储器到存储器的传输

DMA 控制器提供两个AHB 主端口：AHB 存储器端口（用于连接存储器）和 AHB 外设端口（用于连接外设）。但是，要执行存储器到存储器的传输，AHB 外设端口必须也能访问存储器。

两个DMA 控制器的系统实现，见图

![](img/chapter13/1.1.2.png) 

DMA1 控制器AHB 外设端口与DMA2 控制器的情况不同，不连接到总线矩阵，因此，仅DMA2数据流能够执行存储器到存储器的传输。

每个数据流都与一个DMA 请求相关联，此DMA 请求可以从8 个可能的通道请求中选出。此选择由DMA_SxCR 寄存器中的`CHSEL[2:0] `位控制。

下表是DMA2 请求映射

![](img/chapter13/1.1.3.png) 

我们要实现串口 1的 DMA 发送，即 USART1_TX，就必须选择 DMA2 的数据流 7，通道 4，来进行 DMA 传输。

## 电路设计 ##

![](img/chapter0/usb_232_sch.png) 

## 库函数和数据结构 ##

DMA_InitTypeDef结构体的定义：

        typedef struct
        {
            uint32_t DMA_Channel; 
            uint32_t DMA_PeripheralBaseAddr; 
            uint32_t DMA_Memory0BaseAddr; 
            uint32_t DMA_DIR; 
            uint32_t DMA_BufferSize; 
            uint32_t DMA_PeripheralInc; 
            uint32_t DMA_MemoryInc; 
            uint32_t DMA_PeripheralDataSize;
            uint32_t DMA_MemoryDataSize; 
            uint32_t DMA_Mode; 
            uint32_t DMA_Priority; 
            uint32_t DMA_FIFOMode; 
            uint32_t DMA_FIFOThreshold; 
            uint32_t DMA_MemoryBurst; 
            uint32_t DMA_PeripheralBurst; 
        }DMA_InitTypeDef;


        void DMA_Init(DMA_Stream_TypeDef* DMAy_Streamx, DMA_InitTypeDef* DMA_InitStruct);
    
函数的第一个参数是指定初始化的 DMA 的数据流编号，入口参数范围为：y=1,2;x=1-7。第二个参数DMA_InitTypeDef结构体

        void DMA_Cmd(DMA_Stream_TypeDef* DMAy_Streamx, FunctionalState NewState);
    
使能 DMA 数据流的函数。

        void USART_DMACmd(USART_TypeDef* USARTx, uint16_t USART_DMAReq, FunctionalState NewState)；
    
开启串口的 DMA 发送/接送功能，第一个参数为串口编号，第二个为串口的收发功能，第三个参数为使能状态。

        FlagStatus DMA_GetFlagStatus(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG)
    
查询 DMA 传输通道的状态

        void DMA_ClearFlag(DMA_Stream_TypeDef* DMAy_Streamx, uint32_t DMA_FLAG)；
    
清除DMA传输完成标志

        void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);
        
清除串口传输完成标志


## 软件设计、开发 ##

在uart.c文件中添加如下代码：

        #define UART_DMA

        #ifdef UART_DMA

        #define TIMEOUT_MAX 1000 /* 定义最大的溢出数值 */

        void USART_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize) 
        { 
            DMA_InitTypeDef DMA_InitStructure; 
            __IO uint32_t Timeout = TIMEOUT_MAX; 
            /* 复位 DMA Stream 寄存器 (用于调试目的) */ 
            /* DMA_DeInit(USARTx_TX_DMA_STREAM); */ 
            /* 
            使能前检测DMA是否被禁止。请注意，在使用相同数据流的多次传输时，这一步很有用：
            使能，然后禁止，然后使  能..... 在这种情况下，只有在数据传输结束的时候，DMA的禁止才有效，
            在确保使能位被硬件清除之前做确认是不可能的，如果DMA传输只做一次，这一步可以忽略。 
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
            检测DMA Stream 是否被正确的使能.如果DMA的参数配置错误了，
            那么DMA Stream   的使能位会被硬件清除掉，从而使得传输停止比如FIFO的阀值配置错误   */ 
            Timeout = TIMEOUT_MAX; 
            while ((DMA_GetCmdStatus(DMA2_Stream7) != ENABLE) && (Timeout-- > 0)) 
            { 
            } 
            /* 检测是否超时 */ 
            if (Timeout == 0) 
            {	 
                /* 处理错误 */ 

            } 
        } 
        
在main.c文件中添加如下代码：

        //printf("%s",helpstr);

        /* 使能 DMA clock */ 
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 

        /* 使能 USART DMA TX 请求 */ 
        USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 

        USART_DMA_TxConfig((uint32_t *)helpstr, STRLEN); 
       /* 等待数据传输完成，这个函数可以不是用的，直接用下面的DMA等待即可 */ 

        while(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7)==RESET); 
       /* 清除DMA传输完成标志 */ 
       DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7); 
       /* 清除串口传输完成标志 */ 
       USART_ClearFlag(USART1,USART_FLAG_TC); 

## 软件下载、测试验证 ##


