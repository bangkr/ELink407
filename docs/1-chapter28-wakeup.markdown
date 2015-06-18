# 待机唤醒实验 #

通过长按 WAKEUP按键来实现唤醒和进入待机模式的功能，类似手机开关按钮的唤醒功能

## 实验原理与基础知识 ##
默认情况下，系统复位或上电复位后，微控制器进入运行模式。在运行模式下，CPU 通过HCLK 提供时钟，并执行程序代码。系统提供了多个低功耗模式，可在CPU 不需要运行时（例如等待外部事件时）节省功耗。由用户根据应用选择具体的低功耗模式，以在低功耗短启动时间和可用唤醒源之间寻求最佳平衡。

器件有三个低功耗模式：

● 睡眠模式（Cortex™-M4F 内核停止，外设保持运行）

● 停止模式（所有时钟都停止）

● 待机模式（1.2 V 域断电）

此外，可通过下列方法之一降低运行模式的功耗：

● 降低系统时钟速度

● 不使用APBx 和AHBx 外设时，将对应的外设时钟关闭

![](img/chapter15/1.1.1.png) 

## 电路设计 ##

![](img/chapter0/reset_sch.png) 

## 库函数和数据结构 ##
相关的操作在文件 stm32f4xx_pwr.c 和对应的头文件 stm32f4xx_pwr.h 中

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);  
        
使能电源时钟函数，因为要配置电源控制寄存器，所以必须先使能电源时钟。

        PWR_WakeUpPinCmd(ENABLE);   
        
使能唤醒管脚功能函数, 设置 WK_UP 引脚作为唤醒源。使能时钟之后后再设置 PWR_CSR 的 EWUP 位，使能 WK_UP 用于将 CPU 从待机模式唤醒。在库函数中，设置使能 WK_UP 用于唤醒 CPU 待机模式

        void PWR_EnterSTANDBYMode(void)；
        
设置 SLEEPDEEP 位，设置 PDDS 位，执行 WFI 指令，进入待机模式。通过PWR_CR 设置 PDDS 位，使得 CPU 进入深度睡眠时进入待机模式，最后执行 WFI 指令开始进入待机模式，并等待 WK_UP 中断的到来。

        void EXTI0_IRQHandler(void);
        
WakeUP 中断函数。通过 WK_UP 中断（PA0 中断）来唤醒 CPU。

## 软件设计、开发 ##
在工程中添加wakeup.c 文件和wakeup.h 文件，所有待机唤醒相关的驱动代码和定义都在这两个文件中。

编写wakeup GPIO 的初始化代码

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
        
编写进入待机模式函数，当wakeup按键中断产生调用该函数进入低功耗模式

         void StandbyMode(void)
         {	
            //关闭不需要的外设
            RCC_AHB1PeriphResetCmd(0X01FF,ENABLE);  //复位所有 IO 口
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能 PWR 时钟
            PWR_ClearFlag(PWR_FLAG_WU);//清除 Wake-up  标志
            PWR_WakeUpPinCmd(ENABLE);//设置 WKUP 用于唤醒
            PWR_EnterSTANDBYMode();//进入待机模式

         }
         
编写函数 Check_WKUP 实现 WK_UP按下的延时去抖       

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
        
编写wakeup 按键的中断服务函数，当wakeup按键按下产生中断进入待机模式。

        void EXTI0_IRQHandler(void)
        {
            EXTI->PR=1<<0; //清除LINE10上的中断标志位
            if(2==Check_WKUP())//待机?
            {	
                Sys_Enter_Standby();
            }
        }
        
编写main函数进行测试

        int main()
        {
            init_led();

            init_delay();

            init_uart();

            printf("开机唤醒\r\n");

            init_wkup();

            while(1)
            {

                led_on(LED2);

                delay_ms(1000);

                led_off(LED2); 

                delay_ms(1000);	

                printf("主程序while 循环\r\n");

            }

        }

## 软件下载、测试验证 ##


