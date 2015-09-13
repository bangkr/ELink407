# Systick实验 #

在前面的实验中，我们实现了一个延时函数mydelay()，这个函数帮助我们实现了LED闪烁，按键去抖等效果，满足效果是没有问题，但实际使用这样的延时函数是不合适的，它占用了宝贵的CPU资源，而且延时时间不准确，那么在本案例中，我们学习systich部分的知识，完成一个实用的毫秒延时函数，并改写mydelay()函数。

## 实验原理与基础知识 ##

大多操作系统需要一个定时器来产生周期性的中断,作为整个系统的时基，而且最好还让用户程序不能随意访问它的寄存器，以维持操作系统“心跳”的节律。该定时器的时钟源可以是内部时钟（FCLK，上的自由运行时钟），或者是外部时钟（处理器上的 STCLK 信号）。 CM4 内核的处理内部都包含了一个 SysTick 定时器，SysTick 定时器能产生中断，CM4 为它专门开出一个异常类型（异常号：15）并且在向量表中有它的一席之地。SysTick  是一个 24  位的倒计数定时器，当计到 0  时，将从 RELOAD  寄存器中自动重装载定时初值。只要不把它在 SysTick  控制及状态寄存器中的使能位清除，就永不停息。 我们就是利用 STM32 的内部 SysTick 来实现延时的，这样既不占用中断，也不占用系统定时器。

    SysTick control and status register (STK_CTRL) 
    偏移地址：0x00 
        
        
![](img/chapter10/1.1.1.png) 

**Bits 31:17**  保留，但必须保持清零   
**Bit 16** COUNTFLAG:  如果在上次读取本寄存器后，SysTick 已经计到 0，那么该位是 1   
**Bits 15:3**   保留，但必须保持清零   
**Bit 2** CLKSOURCE:  时钟源选择  

 * 0: AHB/8   
 * 1: Processor clock (AHB)   
 
**Bit 1** TICKINT:嘀嗒定时器中断请求使能

 * 1：SysTick 倒数计数到 0 时产生 SysTick 异常请求 
 * 0：数到 0 的时候无动作   
 
**Bit 0** ENABLE:  计数使能   

 * 0 ：计数禁止
 * 1：计数使能   
            
            
    SysTick reload value register (STK_LOAD) 
    偏移地址：0x04 
        
![](img/chapter10/1.1.1.png) 
        
**Bits 31:24**   保留，但必须保持清零 

**Bits 23:0**   RELOAD:重载数值 当使能了计数，并且计数到 0 的时候，这个寄存器的数据将被重新的加载到寄存器。 计算重载数值 RELOAD 寄存器的数值从 0x00000001-0x00FFFFFF，寄存器的数值可以赋值为 0，但是没有影响，只有数值从 1 到 0 的时候嘀嗒定时器的中断请求和计数标志才有效。 

产生一个周期为 N 的多节拍定时器，需要将寄存器 RELOAD 设置为 N-1，比如我们需要嘀嗒定时器每 100 个时钟脉冲产生一个中断，那么 RELOAD=99。 

N 个时钟周期后产生一个滴答中断，需要 RELOAD = 99，比如我们需要嘀嗒定时器每 100个时钟脉冲产生一个中断，那么 RELOAD=99.          
            
    SysTick current value register (STK_VAL) 
    偏移地址：0x08 

![](img/chapter10/1.1.3.png) 

**Bits 31:24**  保留，但必须保持清零 

**Bits 23:0**   CURRENT:当前计数值 读取时返回当前倒计数的值，写它则使之清零，同时还会清除在 SysTick 控制及状态寄存器中的COUNTFLAG 标志。 
        
        
    SysTick calibration value register (STK_CALIB) 
    偏移地址：0x0c 

![](img/chapter10/1.1.4.png) 

**Bit 31**    NOREF:  读取结果为 0 表示分离的参考时钟可用，频率时 HCLK/8 

**Bit 30**    SKEW:  表示 TENMS 的值是否是准确的，校准值是 1ms 不准确的定时是未知的，因为 TENMS 是不知道的，这可能会影响系统定时器是否适合作为一个软件实时时钟 

**Bits 29:24**     保留，但必须保持清零 

**Bits  23:0**      校准值，当嘀嗒定时器运行在 HCLK/8 时表示校准值，该值取决于产品，详细信息请看参考手册的嘀嗒定时器校验值部分，当 HCLK 被编程为最高频率时，嘀嗒定时器的周期是 1ms，如果没有已知的校准信息，需要从核心频率和外部时钟计数校准。

## 电路设计 ##

无

## 库函数和数据结构 ##

相关的函数主要分布在两个文件里面，一个是文件 misc.c 另一个是core_cm4.h。下面我把相关的函数贴出来说明一下。 

        SysTick_Config() 
        
函数SysTick_Config()是属于CMSIS里面的一个函数，实现配置如下 

 * 	- 函数的参数是重载寄存器reload要赋予的初值
 
 *  - 配置嘀嗒定时器的优先级是最低优先级15
 
 *  - 配置HCLK作为系统的时钟源
 
 * 	- 使能嘀嗒定时器中断
 
 * 	- 启动嘀嗒定时器计数 
	
        void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource) 
       
这个函数主要实现嘀嗒定时器时钟源的选择. 用户可以在调用函数SysTick_Config()后通过函数 SysTick_CLKSourceConfig() 更改嘀嗒定时器的时钟源为HCLK/8。SysTick_CLKSourceConfig()在文件misc.c里面 

       NVIC_SetPriority()
       
用户可以在调用函数SysTick_Config()后通过函数 NVIC_SetPriority()修改优先级，函数NVIC_SetPriority()在文件core_cm4.h文件里面 通过下面的公式调整时基 

			Reload Value = SysTick Counter Clock (Hz) x Desired Time base (s)   
            
Reload Value 是函数SysTick_Config()的参数，但是不能超过0xFFFFFF   

这个函数主要是实现嘀嗒定时器的配置，这里面有个对嘀嗒定时器优先级的默认设置，这里将其设置成了15系统最低优先级

## 软件设计、开发 ##

在工程中添加delay.c 文件和delay.h 文件，所有延时相关的函数和定义都在这两个文件中。

(1)定义延时计数全局变量

        unsigned int TimingDelay;

(2)设置嘀嗒定时器是1us中断

        void init_delay()
        {
            if (SysTick_Config(SystemCoreClock / 1000000)) 
            { 
                /* 出错 */ 
                while (1); 
            } 
        }
        
(3)编写异常向量中断服务程序

        void TimingDelay_Decrement(void) 
        { 
            if (TimingDelay != 0) 
            { 
                TimingDelay--; 
            } 
        } 

(4)编写毫秒延时代码

        void delay_ms( unsigned int mTime) 
        { 
            TimingDelay = mTime*1000; 
            while(TimingDelay != 0); 
        }
        
(5)编写微秒延时代码

        void delay_us( unsigned int uTime) 
        { 
            TimingDelay = mTime; 
            while(TimingDelay != 0); 
        }

(6)安装异常向量中断服务程序

打开文件 \基础实验\common\MDK\stm32f4xx_it.c，去掉中断服务程序的注释

        void SysTick_Handler(void)
        {
          //去掉下面函数的注释
            TimingDelay_Decrement();
        }

(7)为了使原来的mydelay()函数能用，改写代码

        void mydelay(int delaytime)
        {
            delay_ms(delaytime);

        #if 0     //使用标准延时函数
            int i;

            for(;delaytime>0;delaytime--)
                        for(i=0;i<1000;i++)
        #endif
                                ;		
        }  

## 软件下载、测试验证 ##
