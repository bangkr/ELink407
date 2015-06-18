# 定时器实验 #
STM32F407的定时器包括TIME1和TIME8两个高级定时器，也有TIME2~TIME5，TIM9~TIM14通用定时器，还有TIME6和TIME7等基本定时器，总共达14个定时器之多。在案例将使用TIM3的定时器中断，设置定时器时间为1秒，并在定时器中断处理函数中通过串口输出到时消息。

## 实验原理与基础知识 ##
STM32F4的通用定时器包含一个16位或32位自动重载计数器（CNT），该计数器由可编程预分频器（PSC）驱动。
STM32F4的通用定时器可以被用于：测量输入信号的脉冲长度(输入捕获)或者产生输出波形(输出比较和PWM)。  
使用定时器预分频器和RCC时钟控制器预分频器，脉冲长度和波形周期可以在几个微秒到几个毫秒间调整。
STM32F4的每个通用定时器都是完全独立的，没有互相共享的任何资源。 
STM3的通用TIMx (TIM2~TIM5和TIM9~TIM14)定时器功能包括：   
    1)16位/32 位(仅TIM2和TIM5)向上、向下、向上/向下自动装载计数器（TIMx_CNT），注意：TIM9~TIM14只支持向上（递增）计数方式。 
    2)16 位可编程(可以实时修改)预分频器(TIMx_PSC)，计数器时钟频率的分频系数为1～65535之间的任意数值。 
    3）4个独立通道（TIMx_CH1~4，TIM9~TIM14最多2个通道），这些通道可以用来作为：   
        A．输入捕获   
        B．输出比较   
        C．PWM生成(边缘或中间对齐模式) ，注意：TIM9~TIM14不支持中间对齐模式 
        D．单脉冲模式输出   
        4）可使用外部信号（TIMx_ETR）控制定时器和定时器互连（可以用1个定时器控制另外一个定时器）的同步电路。 
    5）如下事件发生时产生中断/DMA（TIM9~TIM14不支持DMA）：   
        A．更新：计数器向上溢出/向下溢出，计数器初始化(通过软件或者内部/外部触发)  
        B．触发事件(计数器启动、停止、初始化或者由内部/外部触发计数)  
        C．输入捕获   
        D．输出比较   
        E．支持针对定位的增量(正交)编码器和霍尔传感器电路（TIM9~TIM14不支持）   
        F．触发输入作为外部时钟或者按周期的电流管理（TIM9~TIM14不支持） 

## 电路设计 ##

## 库函数和数据结构 ##
使能TIM3时钟：
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
初始化定时器参数,设置自动重装值，分频系数，计数方式等：
        voidTIM_TimeBaseInit(TIM_TypeDef*TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); 
        第一个参数是确定是哪个定时器，这个比较容易理解。第二个参数是定时器初始化参数结构体指针，结构体类型为TIM_TimeBaseInitTypeDef，下面我们看看这个结构体的定义： 
        typedef struct                                         
        { 
          uint16_t TIM_Prescaler;      
          uint16_t TIM_CounterMode;        
          uint16_t TIM_Period;          
          uint16_t TIM_ClockDivision;    
          uint8_t TIM_RepetitionCounter;     
        } TIM_TimeBaseInitTypeDef;              
        对于通用定时器只有前面四个参数有用，最后一个参数TIM_RepetitionCounter是高级定时器才有用的。 
        TIM_Prescaler：用来设置分频系数 
        TIM_CounterMode：用来设置计数方式。
        TIM_Period：设置自动重载计数周期值 
        TIM_ClockDivision：用来设置时钟分频因子。 
        
设置TIM3_DIER允许更新中断：
        TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); 
        
TIM3中断优先级设置：
        NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断 
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1 
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //响应优先级3 
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; 
        NVIC_Init(&NVIC_InitStructure);//  ④初始化NVIC  
        
开启TIM3定时器： 
        TIM_Cmd(TIM3, ENABLE);    //使能TIM3外设 

编写中断服务函数：
        //判断定时器3是否发生更新（溢出）中断 
        if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){} 
        //清除定时器TIMx的中断TIM_IT标志位
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update
        
## 软件设计、开发 ##
1)  初始化通用TIM3 
        //arr：自动重装值。  psc：时钟预分频数 
        //定时器到时时间计算方法:Tout=((arr+1)*(psc+1)) 
        void timer_init(u16 arr,u16 psc) 
        { 
            TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
            NVIC_InitTypeDef NVIC_InitStructure; 
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //使能TIM3时钟 

            TIM_TimeBaseInitStructure.TIM_Period = arr;  //自动重装载值 
            TIM_TimeBaseInitStructure.TIM_Prescaler=psc;    //定时器分频 
            TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
            TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;  

            TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);// 初始化定时器TIM3 

            TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断 

            NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断 
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1 
            NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //响应优先级3 
            NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; 
            NVIC_Init(&NVIC_InitStructure);// 初始化NVIC  
            TIM_Cmd(TIM3,ENABLE); //使能定时器3  
        }
        
2）定时器中断处理函数

        void TIM3_IRQHandler(void) 
        { 
            if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断 
            { 
                printf("timer out !\r\n");
            } 
            TIM_ClearITPendingBit(TIM3,TIM_IT_Update);    //清除中断标志位 
        }
        
3）编写主函数 

        int main()
        {
            uart_config();
            //Tout=((arr+1)*(psc+1)),
            
            //84M等于APB1定时器时钟主频，即每秒产生一次定时器中断 
            timer_init(10000-1,8400-1) ;
            while(1);
        }
    
## 软件下载、测试验证 ##

