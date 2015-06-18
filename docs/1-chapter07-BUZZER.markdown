# 蜂鸣器实验 #

上一章，我们介绍了 STM32F4 的 IO 口作为输出的使用，这一章我们继续巩固前面的知识，通过GPIO控制蜂鸣器，希望大家看完本章的实验原理与基础知识和电路设计部分的内容后能独立完成驱动蜂鸣器驱动程序的编写。

## 实验原理与基础知识 ##

蜂鸣器是一种一体化结构的电子讯响器，采用直流电压供电，广泛应用于计算机、打印机、复印机、报警器、电子玩具、汽车电子设备、电话机、定时器等电子产品中作发声器件。蜂鸣器主要分为压电式蜂鸣器和电磁式蜂鸣器两种类型。

这里的有源不是指电源的“源”，而是指有没有自带震荡电路，有源蜂鸣器自带了震荡电路，一通电就会发声；无源蜂鸣器则没有自带震荡电路，必须外部提供 2~5Khz 左右的方波驱动，才能发声。

ELink407开发板板载的蜂鸣器是电磁式的有源蜂鸣器，如图所示：

![](img/elink407/beep.png) 

## 电路设计 ##

STM32F4 的单个 IO 最大可以提供 25mA 电流，而蜂鸣器的驱动电流是 30mA左右，考虑STM32F4 整个芯片的电流，最大也就 150mA，如果用IO 口直接驱动蜂鸣器，其他地方用电就会不足，所以我们不用 STM32F4 的 IO 直接驱动蜂鸣器，而是通过三极管扩流后再驱动蜂鸣，这样 STM32F4 的 IO 只需要提供不到 1mA 的电流就足够了。所以我们的硬件电路设计如下：

![](img/chapter0/beep_sch.png) 

从电路原理图上，我们可以看到BEEP 和 ADC0 复用了PC0 管脚，在开发板上有一个跳线J34，如果我们需要使用蜂鸣器，需要跳线帽将J34的 1，2脚短接。

## 库函数和数据结构 ##

跟GPIO使用库函数一样

## 软件设计、开发 ##

首先我们复制我们的第一个工程firstPrj文件夹，重命名为buzzer，修改工程文件名为buzzer.uvproj。双击打开工程，在菜单-->Project-->option --> Output选项卡 中 配置Name of Executable 为buzzer。

在固件库中，GPIO 端口操作对应的库函数函数以及相关定义在文件stm32f4xx_gpio.h 和 stm32f4xx_gpio.c 中，所以drv文件组中，我们只需要保留，stm32f4xx_gpio.c，stm32f4xx_rcc.c两个文件即可。

(1) buzzer.h

        #ifndef __BUZZER_H
        #define __BUZZER_H 

        void init_buzzer(void);
        void buzzer_on(void);
        void buzzer_off(void);

        #endif
        
(2) buzzer.c

        #include "buzzer.h"
        #include "stm32f4xx.h"

        void buzzer_led(void)
        {
            GPIO_InitTypeDef  GPIO_InitStructure;

            /* Enable the GPIO_BUZZER Clock */
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

            /* Configure the GPIO_BUZZER pin */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_Init(GPIOC, &GPIO_InitStructure);

            GPIO_SetBits(GPIOC,GPIO_Pin_0);       //配置蜂鸣器 不响

        }

        void buzzer_on(void)
        {
                GPIO_ResetBits(GPIOC,GPIO_Pin_0); //蜂鸣器响 
        }

        void buzzer_off(void)
        {
                GPIO_SetBits(GPIOC,GPIO_Pin_0);   //不响 
        }

将编写好的buzzer驱动分别保存在 \基础实验\bsp4Elink\inc\, \基础实验\bsp4Elink\src\ 目录中。

(3) 编写案例测试代码main.c
 
        #include "buzzer.h"
        #include "delay.h"

        int main(void)
        {
            init_buzzer();

            while(1)
            {
                buzzer_on();
                mydelay(1000);

                buzzer_off();
                mydelay(10000);
            }
        }


## 下载测试 ##

