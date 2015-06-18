# 窗口看门狗实验 #

和独立看门狗不一样的地方是，窗口看门狗可以触发中断，本实验我们将使用窗口看门狗的中断功能来喂狗。

## 实验原理与基础知识 ##

窗口看门狗(WWDG) 时钟由APB1 时钟经预分频后提供，通过可配置的时间窗口来检测应用程序非正常的过迟或过早的操作。WWDG 最适合那些要求看门狗在精确计时窗口起作用的应用程序

窗口看门狗通常被用来监测，由外部干扰或不可预见的逻辑条件造成的应用程序背离正常的运行序列而产生的软件故障。除非递减计数器的值在T6 位变成0 前被刷新，看门狗电路在达到预置的时间周期时，会产生一个MCU 复位。如果在递减计数器达到窗口寄存器值之前刷新控制寄存器中的7 位递减计数器值，也会产生MCU 复位。这意味着必须在限定的时间窗口内刷新计数器。

窗口看门狗框图

![](img/chapter15/1.1.1.png) 

窗口看门狗时序图

![](img/chapter15/1.1.2.png) 

窗口看门狗的 3 个寄存器

    控制寄存器(WWDG_CR)      Status register
    
    偏移地址：0x00
    
![](img/chapter15/1.1.3.png) 

    配置寄存器(WWDG_CFR)     Configuration register
    
    偏移地址：0x04
    
![](img/chapter15/1.1.4.png) 

    状态寄存器(WWDG_SR)      Status register
    
    偏移地址：0x08
    
![](img/chapter15/1.1.5.png) 
    

## 电路设计 ##

## 库函数和数据结构 ##
窗口看门狗库函数相关源码和定义分布在文件 stm32f4xx_wwdg.c 文件和头文件 stm32f4xx_wwdg.h 中

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);   
        
WWDG 不同于 IWDG，IWDG 有自己独立的 32Khz 时钟，不存在使能问题。而 WWDG使用的是 PCLK1 的时钟，需要先使能时钟。

        void WWDG_SetWindowValue(uint8_t WindowValue)；
        
设置窗口值的函数,一个参数为窗口值。

        void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)；
        
设置分频数的函数,一个参数就是分频值。

        WWDG_EnableIT(); //开启窗口看门狗中断
        
开启 WWDG 中断的函数。

        void WWDG_Enable(uint8_t Counter)；
        
设置计数器初始值并使能看门狗函数

        void WWDG_SetCounter(uint8_t Counter);
        
独立的设置计数器值的函数

## 软件设计、开发 ##
在工程中添加wwdg.c文件和wwdg.h 文件，所有 WWDG 相关的驱动代码和定义都在这两个文件中。

## 软件下载、测试验证 ##


