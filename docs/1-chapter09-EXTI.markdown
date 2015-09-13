# 按键中断实验 #   

上个一实验，我们通过CPU不断读取按键GPIO 输入寄存器的值，查询是否有按键按下，并做相应的控制操作，但是这种轮询的方式存在两个问题，首先浪费CPU的资源，无论按键是否发生，CPU都要做查询操作。其次，如果CPU的任务比较繁多，CPU有可能不能及时响应按键的请求，甚至不响应。解决这个问题，我们引入了中断机制，当按键按下以后，产生一个中断请求，CPU收到请求后，中断当前用户程序，转而执行中断响应代码。下面的案例就是在ELink407上实现按键中断控制LED灯。

    ## 实验原理与基础知识 ##

STM32F4 的每个 IO 都可以作为外部中断的中断输入口。 STM32F407 的中断控制器支持 22 个外部中断/事件请求。每个中断设有状态位，每个中断/事件都有独立的触发和屏蔽设置。 STM32F407的 22 个外部中断为：

EXTI 线 0~15：对应外部 IO 口的输入中断。

EXTI 线 16：连接到 PVD 输出。

EXTI 线 17：连接到 RTC 闹钟事件。

EXTI 线 18：连接到 USB OTG FS 唤醒事件。

EXTI 线 19：连接到以太网唤醒事件。

EXTI 线 20：连接到 USB OTG HS(在 FS 中配置)唤醒事件。

EXTI 线 21：连接到 RTC 入侵和时间戳事件。

EXTI 线 22：连接到 RTC 唤醒事件。

从上面可以看出，STM32F4 供 IO 口使用的中断线只有 16 个，但是 STM32F4 的 IO 口却远远不止 16 个，那么 STM32F4 是怎么把 16 个中断线和 IO 口一一对应起来的呢？于是 STM32就这样设计， GPIO 的管脚 GPIOx.0~GPIOx.15 (x=A,B,C,D,E， F,G,H,I)分别对应中断线 0~15。这样每个中断线对应了最多 9 个 IO 口，以线 0 为例：它对应了 GPIOA.0、GPIOB.0、GPIOC.0、GPIOD.0、GPIOE.0、GPIOF.0、GPIOG.0,GPIOH.0,GPIOI.0。而中断线每次只能连接到 1 个 IO口上，这样就需要通过配置来决定对应的中断线配置到哪个 GPIO 上了。下面我们看看 GPIO跟中断线的映射关系图： 

![](img/chapter9/1.1.1.png) 

要产生中断，必须先配置好并使能中断线。根据需要的边沿检测设置2 个触发寄存器，同时在中断屏蔽寄存器的相应位写“1”使能中断请求。当外部中断线上出现选定信号沿时，便会产生中断请求，对应的挂起位也会置1。在挂起寄存器的对应位写“1”，将清除该中断请求。

## 电路设计 ##

![](img/chapter0/key_sch.png) 

## 库函数和数据结构 ##

本案例涉及的库函数函及相关定义在文件stm32f4xx_gpio.h,stm32f4xx_rcc.h，stm32f4xx_exti.h和 stm32f4xx_gpio.c, stm32f4xx_rcc.c,stm32f4xx_exti.c中。

结构体:

        typedef struct
        {
            /*!< Specifies the IRQ channel to be enabled or disabled.*/               
          uint8_t NVIC_IRQChannel;                    
          
          /*!< Specifies the pre-emption priority for the IRQ channel*/
          uint8_t NVIC_IRQChannelPreemptionPriority;  
          
          /*!< Specifies the subpriority level for the IRQ channel */
          uint8_t NVIC_IRQChannelSubPriority;         
          
          /*!< Specifies whether the IRQ channel defined in NVIC_IRQChannel*/
          FunctionalState NVIC_IRQChannelCmd;
        } NVIC_InitTypeDef;

        typedef struct
        {
            /*!< Specifies the EXTI lines to be enabled or disabled.*/
          uint32_t EXTI_Line;               

          /*!< Specifies the mode for the EXTI lines.*/
          EXTIMode_TypeDef EXTI_Mode;       

          /*!< Specifies the trigger signal active edge for the EXTI lines.*/
          EXTITrigger_TypeDef EXTI_Trigger; 

          /*!< Specifies the new state of the selected EXTI lines.*/
          FunctionalState EXTI_LineCmd;     
        }EXTI_InitTypeDef;
        
EXTI_Line中断线的标号，对于我们的外部中断，取值范围为 EXTI_Line0~EXTI_Line15。这个在上面已经讲过中断线的概念。也就是说，这个函数配置的是某个中断线上的中断参数。

EXTI_Mode中断模式，可选值为中断EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。

EXTI_Trigger触发方式，可以是下降沿触发EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发 EXTI_Trigger_Rising_Falling，。

EXTI_LineCmd使能中断线为DISABLE或 ENABLE。

库函数:

        void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
        
开启 SYSCFG 时钟函数，SYSCFG 是挂载在 APB2 时钟下,所以要使用RCC_APB2PeriphClockCmd这个函数。 第一个参数为外设名称，可以在stm32f4xx_rcc.h文件中找到 RCC_AHP2Periph外设的定义。第二个参数为DISABLE或 ENABLE 

        void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex);
        
配置 GPIO 与中断线的映射。

        void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
        
中断线上中断的初始化函数。 

        void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);                           
        
中断优先级分组初始化函数

        ITStatus EXTI_GetITStatus(uint32_t EXTI_Line)；
        
这个函数一般使用在中断服务函数的开头判断中断是否发生。

        void EXTI_ClearITPendingBit(uint32_t EXTI_Line)；
        
清除某个中断线上的中断标志位函数

## 软件设计、开发 ##
在工程中添加button.c 文件和button.h 文件，按键中断相关的函数和定义都在这两个文件中。

使用 IO 口外部中断的步骤：

1)  使能 IO 口时钟，初始化 IO 口为输入首先，我们要使用 IO 口作为中断输入，所以我们要使能相应的 IO 口时钟，以及初始化相应的 IO 口为输入模式，具体的使用方法跟我们按键实验是一致的。然后开启 SYSCFG 时钟，

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能 SYSCFG 时钟
        
2）使能 SYSCFG 时钟，设置 IO 口与中断线的映射关系。

        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
        SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);

3）初始化线上中断，设置触发条件等。

        EXTI_InitTypeDef    EXTI_InitStructure;
        EXTI_InitStructure.EXTI_Line=EXTI_Line4;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);      //初始化外设 EXTI 寄存器

4）配置中断分组（NVIC），并使能中断。

        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;     //使能按键外部中断通道
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;  //抢占优先级 2， 
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;    //响应优先级 2
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      //使能外部中断通道
        NVIC_Init(&NVIC_InitStructure);                      //中断优先级分组初始化

5）编写中断服务函数。

中断服务函数的名字是在标准库中事先有定义的。这里需要说明一下， STM32F4 的 IO 口外部中断函数只有 7 个，分别为：

        EXPORT   EXTI0_IRQHandler          
        EXPORT   EXTI1_IRQHandler       
        EXPORT   EXTI2_IRQHandler         
        EXPORT   EXTI3_IRQHandler          
        EXPORT   EXTI4_IRQHandler           
        EXPORT   EXTI9_5_IRQHandler        
        EXPORT   EXTI15_10_IRQHandler      
        
中断线 0-4 每个中断线对应一个中断函数，中断线 5-9 共用中断函数 EXTI9_5_IRQHandler，中断线 10-15 共用中断函数 EXTI15_10_IRQHandler。我们以按键1和按键2中断服务程序为例：

按键1对应GPIOD_pin_6，所以中断服务程序名称为EXTI9_5_IRQHandler

        void EXTI9_5_IRQHandler(void)
         {
            if(EXTI_GetITStatus(EXTI_Line6)!=RESET)//判断某个线上的中断是否发生 
            {   
                //…中断处理…
                led_on()
                EXTI_ClearITPendingBit(EXTI_Line6);   //清除 LINE 上的中断标志位
            }     
        }
 
按键1对应GPIOE_Pin_2，所以中断服务程序名称为EXTI2_IRQHandler

        void EXTI2_IRQHandler(void)
        {
            if(EXTI_GetITStatus(EXTI_Line2)!=RESET)//判断某个线上的中断是否发生 
            {   
                //…中断处理…
                led_on()
                EXTI_ClearITPendingBit(EXTI_Line2);   //清除 LINE 上的中断标志位
            }     
        }
        
(6) 编写main函数进行测试

        #include "led.h"
        #include "delay.h"
        #include "button_noexti.h"

        #include "button.h"

        int main()
        {
            init_led();

            NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组 2

            EXTIX_Init();       //初始化外部中断输入


                while(1)
                {

                   mydelay(1000);

               }
        }

## 软件下载、测试验证 ##

