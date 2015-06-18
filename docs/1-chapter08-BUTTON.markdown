# 按键实验 #

前面的实验我们学会了，通过STM32F407输出信息，控制外部设备（LED，BUZZER）。那么CPU从外界获取信息的过程又是怎么样的呢？本案例我们让大家实现通过按键输入来控制LED灯。

## 实验原理与基础知识 ##

在前面的案例中，我们是通过库函数 GPIO_ResetBits()和GPIO_SetBits()来设置两个输出数据寄存器。STM32F4 的 IO口做输入使用的时候，是通过调用函数 GPIO_ReadInputDataBit()来读取 IO 口的状态的。那么可以开始我们的代码编写了。
 
## 电路设计 ##

![](img/chapter0/key_sch.png) 

## 库函数和数据结构 ##

本案例涉及的库函数函及相关定义在文件stm32f4xx_gpio.h,stm32f4xx_rcc.h和 stm32f4xx_gpio.c, stm32f4xx_rcc.c 中。

库函数:

        uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

函数读取 IDR 寄存器组内管脚上电平值，第一个参数为GPIOx组寄存器的起始地址，第二个参数为组内GPIO编号。

## 软件设计、开发 ##

在工程中添加button_noexti.c 文件和button_noexti.h 文件，按键相关的函数和定义都在这两个文件中。

(1) 初始化GPIOD，GPIOE 上的PIN,配置为输入模式

        void init_button(void)
        {
            GPIO_InitTypeDef  GPIO_InitStructure;

            /* Enable the GPIO_BUTTON Clock */
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

            /* Configure the GPIO_BUTTON pin */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 |GPIO_Pin_4 ;
            GPIO_Init(GPIOE, &GPIO_InitStructure);

        }
        
(2)读取按键状态

        int button_read(void)
        {
            int status=0;

            status=  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)<<3
                         | GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)<<2
                         | GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)<<1
                         | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)<<0;

            return (~status) &0xF;

        }

(3) 编写测试程序

        #include "stm32f4xx.h"
        #include "led.h"
        #include "delay.h"

        #include "button_noexti.h"

        int main()
        {
            int button_value=0;

            init_led();

            while(1)
            {

                button_value=button_read();

                if(button_value>0)
                {
                    mydelay(10);		//延时去抖

                    if(button_value==button_read())
                    {
                            switch(button_value)
                            {
                                case 1:
                                    led_on(LED1);
                                    break;

                                case 2:
                                    led_on(LED2);
                                    break;

                                case 4:
                                    led_on(LED1);
                                    led_on(LED2);
                                    break;

                                case 8:
                                    led_off(LED1);
                                    led_off(LED2);
                                    break;

                                default:			//两个键同时按下，不处理
                                    break;
                            }
                        }
                    }
                }
            }

## 软件下载、测试验证 ##
 
 