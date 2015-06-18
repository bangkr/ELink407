# UART 实验 #

UART是一种通用串行数据总线，用于异步通信。该总线双向通信，可以实现全双工传输和接收。在嵌入式设计中，UART常用来主机与辅助设备通信，另外通过串口信息对代码进行调试也是常用手段之一。

## 实验原理与基础知识 ##

stm32f407系列，串口有3种工作模式：查询、中断、DMA。查询最简单，效率也最低，因为mcu速度很快，而串口发送效率很低，所以实际项目中基本不用。中断模式效率高，但如果发送/接收大量字节，会产生大量中断，影响整个系统效率。DMA方式适合于高速设置的数据传输，如果应用要求Uart 的速度不是很高不建议采用，另外使用DMA方式会占用DMA通道，stm32如果其它设备占用了DMA通道，那么久不能使用。

本案例采用中断方式，利用st库函数对编写UART1串口驱动，并通过重写fputc函数，实现使用printf函数打印串口消息。

**USART_SR 寄存器**


这里我们关注一下两个位，第 5、6 位 RXNE 和 TC。

RXNE（读数据寄存器非空），当该位被置 1 的时候，就是提示已经有数据被接收到了，并且可以读出来了。这时候我们要做的就是尽快去读取 USART_DR，通过读 USART_DR 可以将该位清零，也可以向该位写 0，直接清除。

TC （发送完成），当该位被置位的时候，表示 USART_DR 内的数据已经被发送完成了。如果设置了这个位的中断，则会产生中断。该位也有两种清零方式：1）读 USART_SR，写USART_DR。2）直接向该位写 0。


## 电路设计 ##

UART1串口对应收发引脚连接在开发GPIO的PA9和PA10，PA9,PA10经过USB转串口电路，跟PC用mini USB线连接，如图

![](img/chapter0/usb_232_sch.png) 


我们把的UART1 UART3 的Pin也扩展出来了，可以参看原理图J9,J12。

![](img/chapter0/485_sch.png) 


## 库函数和数据结构 ##
串口基本配置直接相关的几个固件库函数。这些函数和定义主要分布在 stm32f4xx_usart.h 和 stm32f4xx_usart.c 文件中。

        RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
        
串口是挂载在 APB2 下面的外设，所以使用RCC_APB2PeriphClockCmd()函数：

        void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF);
        
设置引脚复用器映射函数，对于 GPIO_PinAFConfig 在相应的配置文件找到外设对应的 AF 配置宏定义标识符即可，宏定义在文件 stm32f4xx_gpio.h中，串口 1 为 GPIO_AF_USART1。

        void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct)
        
串口参数初始化：设置波特率，字长，奇偶校验等参数

        void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);   
        
使能串口函数，将 USART_CR1 寄存器的 UE 置 1 来激活 USART。
    
        void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
        
库函数操作 USART_DR 寄存器发送数据的函数，通过该函数向串口寄存器 USART_DR 写入一个数据。

        uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
        
库函数操作 USART_DR 寄存器读取串口接收到的数据的函数，通过该函数可以读取串口接受到的数据。

## 软件设计、开发 ##
在工程中添加uart.c 文件和uart.h 文件，所有 uart相关的驱动代码和定义都在这两个文件中。

1)  串口时钟和 GPIO 时钟使能。

串口是挂载在 APB2 下面的外设，所以使能函数为：

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能 USART1 时钟
        
GPIO 时钟使能，串口 1 对应着芯片引脚 PA9,PA10，所以这里我们需要使能 GPIOA 时钟：

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能 GPIOA 时钟
        
2)  设置引脚复用器映射

PA9 和 PA10 都映射到串口 1，引脚复用器映射，调用函数为：

        GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //PA9 复用为 USART1
        GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);//PA10 复用为 USART1
        
对于 GPIO_PinAFConfig 在相应的配置文件找到外设对应的 AF 配置宏定义标识符即可，串口 1 为 GPIO_AF_USART1。

3)  GPIO 端口模式设置：PA9 和 PA10 要设置为复用功能。

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9 与 GPIOA10
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //速度 50MHz
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
        GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化 PA9，PA10
        
4)  串口参数初始化：设置波特率，字长，奇偶校验等参数

串口初始化是调用函数 USART_Init 来实现的，具体设置方法如下：

        USART_InitStructure.USART_BaudRate = bound;//设置为 115200;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为 8 位数据格式
        USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
        USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
        USART_Init(USART1, &USART_InitStructure); //初始化串口
        
5)  调用函数 USART_Cmd 使能串口 1 方法如下：

        USART_Cmd(USART1, ENABLE);   //使能串口
    
6) 引入 printf 和scanf 函数支持需要 重定义 fputc() 和 fgetc()函数
 
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

7)  清发送完成标志

        USART_ClearFlag(USART1, USART_FLAG_TC); 
        
## 软件下载、测试验证 ##

