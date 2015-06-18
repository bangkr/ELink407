# LED灯实验 #

我们常常通过观察设备电源指示灯（LED灯）状态来判断设备是否加电或者设备供电是否正常，通过这一直观的现象我们很容易获得计算机运行的输出信息，这是一种非常简单也是常用的人机交互的方式，本次实验我们就需要实现计算机控制LED灯的交替闪烁。

## 实验原理与基础知识 ##

LED是发光二极管的简称，它是半导体二极管的一种，可以把电能转化成光能。发光二极管与普通二极d管一样是由一个PN结组成，也具有单向导电性。当给发光二极管加上正向电压后，从P区注入到N区的空穴和由N区注入到P区的电子，在PN结附近数微米内分别与N区的电子和P区的空穴复合，产生自发辐射的荧光。不同的半导体材料中电子和空穴所处的能量状态不同。当电子和空穴复合时释放出的能量多少不同，释放出的能量越多，则发出的光的波长越短。波长不一样那么发光颜色就不一样。
   
![](img/chapter06/1.1.1.jpg) 

那么我们这个LED灯的实验，我们怎么通过编码（或者说指令）控制LED灯的状态呢？我们可以先看一下我们LED部分的原理图

我们可以看到LED1，LED2 的P端接电源的正极，N端分别接在我们的STM32F407VGT6芯片的PA8，PB8管脚上，根据LED的发光原理，如果管脚上为低电平，PN两端产生压差，电子移动，发生电子从低轨道到高轨道的跃迁，释放能量，LED发光。如果管脚上为高电平，PN结两端没有压差，没有电子跃迁，LED熄灭。

所以，对LED灯的控制就变成了，对PA8，PB8管脚输出电平的控制了，那么在STM32F407VGT6芯片上，如果控制管脚的呢？下面我们开启STM32F407 GPIO的学习。

所谓GPIO就是通用输入输出管脚（General Purpose Input Output），是芯片与外部设备进行通信的基本方式，打开芯片手册文档或我们的原理图，我们可以看到STM32F407VGT6总共有6组GPIO（不同的芯片型号GPIO管脚数量可能不一样，组数也不一样）分别是PA，PB，PC，PD，PE，PH 。PH组只有两个管脚，其他组有16个管脚，STM32F4 每组通用  I/O  端口包括  4  个  32  位配置寄存器（MODER、 OTYPER、 OSPEEDR 和  PUPDR）、 2  个  32  位数据寄存器（IDR ODR）、  2  个  32  位复用功能选择寄存器（AFRH  和  AFRL）和 1  个  32  位置位/复位寄存器  (BSRR)、1  个  32  位锁定寄存器  (LCKR)  。这样， STM32F4 每组 IO 有 10 个 32 位寄存器控制，其中常用的有 4 个配置寄存器+2 个数据寄存器+2 个复用功能选择寄存器，共 8 个。

接下来我们详细介绍 这8个常用的寄存器：  MODER、 OTYPER、 OSPEEDR、 PUPDR、 ODR、IDR  、 AFRH 和 AFRL。介绍完后我们就可以通过对这些寄存器的操作，来编写LED的驱动代码。[先使用自己操作寄存器（自己的代码操作寄存器）的方法来配置，稍后我们讲解对应的库函数配置方法。]

**MODER 寄存器**  

该寄存器是 GPIO 端口模式控制寄存器，用于控制 GPIOx（STM32F4 最多有 9 组 IO，分别用大写字母表示，即x=A/B/C/D/E/F/G/H/I，下同）寄 存 器 用 于 选择I/O 方向（输入、输出、AF、模拟），该寄存器各位描述如表所示：

![](img/chapter06/1.1.4.png) 

该寄存器各位在复位后，一般都是 0 （个别不是 0，比如 JTAG 占用的几个 IO 口），也就是默认条件下一般是输入状态的。每组 IO 下有 16 个 IO 口，该寄存器共 32 位，每 2 个位控制 1个 IO。

**OTYPER 寄存器**

该寄存器用于控制 GPIOx 的输出类型（推挽或开漏），该寄存器各位描述见表所示：

![](img/chapter06/1.1.5.png) 

该寄存器仅用于输出模式，在输入模式（MODER[1:0]=00/11 时）下不起作用。该寄存器低 16 位有效，每一个位控制一个 IO 口，复位后，该寄存器值均为 0。

**OSPEEDR寄存器** 

该寄存器用于控制 GPIOx 的输出速度，无论采用哪种I/O 方向，都会直接将I/O 速度引脚连接到相应的GPIOx_OSPEEDR 寄存器位，该寄存器各位描述见表所示：

![](img/chapter06/1.1.6.png) 

该寄存器也仅用于输出模式，在输入模式（MODER[1:0]=00/11 时）下不起作用。该寄存器每 2 个位控制一个 IO 口，复位后，该寄存器值一般为 0。


**PUPDR寄存器** 

该寄存器用于控制 GPIOx 的上拉/下拉，该寄存器各位描述见表所示：

![](img/chapter06/1.1.7.png) 

该寄存器每 2 个位控制一个 IO 口，用于设置上下拉，复位后，该寄存器值一般为 0。


**ODR 寄存器** 

该寄存器用于控制 GPIOx 的输出，该寄存器各位描述见表所示：

![](img/chapter06/1.1.8.png) 

该寄存器用于设置某个 IO 输出低电平(ODRy=0)还是高电平(ODRy=1)，该寄存器也仅在输出模式下有效，在输入模式（MODER[1:0]=00/11 时）下不起作用。。

**IDR 寄存器** 

该寄存器用于读取 GPIOx 的输入，该寄存器各位描述见表所示：

![](img/chapter06/1.1.9.png) 
该寄存器用于读取某个 IO 的电平，如果对应的位为 0(IDRy=0)，则说明该 IO 输入的是低电平，如果是 1(IDRy=1)，则表示输入的是高电平。


**AFRL 和 AFRH 寄存器** 

这两个寄存器用来设置 IO 引脚复用和映射的，STM32F4 每个 IO 引脚通过一个复用器连接到板载外设/模块，该复用器一次仅允许一个外设的复用功能  (AF)  连接到IO  引脚如图1.1.10，这样，可以确保共用同一个  IO  引脚的外设之间不会发生冲突。引脚复用功能选择，正是通过 AFRL 和 AFRH 来控制的，其中 AFRL 控制0~7 这 8 个 IO 口，AFRH 控制 8~15 这 8 个 IO 口。

![](img/chapter06/1.1.12.png) 

AFRL 寄存器各位描述如图所示： 

![](img/chapter06/1.1.10.png) 

AFRH 寄存器各位描述如图所示： 

![](img/chapter06/1.1.11.png) 

GPIO的寄存器介绍的差不多了，接下来，我们就要结合我们的开发板LED部分的原理图编写驱动代码了


## 电路设计 ##

如图LED部分的电路设计，电路的工作原理我们在上一小节中已经说明，这里不再讲解，在这里再次给出原理图，希望大家注意LED1，LED2对应的GPIO管脚，以便于下面我们设计其驱动代码。

![](img/chapter06/1.1.2.png) 

## 不使用标准库，驱动代码设计 ##
通过对GPIO寄存器的了解，我们知道控制一个LED灯闪烁大概需要一下几个步骤，首先是配置管脚作为输出，然后输出低电平，LED导通发光，延时一段时间后，我们再关闭LED，即将相应管脚输出高电平，管脚上高低电平如此反复，LED灯产生闪烁。

双击Keil5 新建工程，在这里我们使用的是直接操作GPIO寄存器，所以不需要拷贝我们第一个工程模版（工程模版我们会在用标准库的操作寄存器的时候使用），点击新建文件按钮，我们开始编写我们的头文件：

        #define     __IO    volatile            

        #define GPIOA_BASE    ((uint32_t)0x40020000) 
        #define GPIOB_BASE    ((uint32_t)0x40020400) 

        typedef unsigned  int 		uint32_t;
        typedef unsigned short     int uint16_t;

        typedef struct
        {
          __IO uint32_t MODER;    /*!< GPIO port mode register,               offset: 0x00      */
          __IO uint32_t OTYPER;   /*!< GPIO port output type register,        offset: 0x04      */
          __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       offset: 0x08      */
          __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  offset: 0x0C      */
          __IO uint32_t IDR;      /*!< GPIO port input data register,         offset: 0x10      */
          __IO uint32_t ODR;      /*!< GPIO port output data register,        offset: 0x14      */
          __IO uint16_t BSRRL;    /*!< GPIO port bit set/reset low register,  offset: 0x18      */
          __IO uint16_t BSRRH;    /*!< GPIO port bit set/reset high register, offset: 0x1A      */
          __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, offset: 0x1C      */
          __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     offset: 0x20-0x24 */
        } GPIO_TypeDef;

        #define GPIOA_Reg            ((GPIO_TypeDef *) GPIOA_BASE)
        #define GPIOB_Reg            ((GPIO_TypeDef *) GPIOB_BASE)

        #define RCC_BASE    			((uint32_t)0x40023800) 

        typedef struct
        {
          __IO uint32_t CR;            /*!< RCC clock control register,         offset: 0x00 */
          __IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,     offset: 0x04 */
          __IO uint32_t CFGR;          /*!< RCC clock configuration register,   offset: 0x08 */
          __IO uint32_t CIR;           /*!< RCC clock interrupt register,       offset: 0x0C */
          __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register, offset: 0x10 */
          __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register, offset: 0x14 */
          __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register, offset: 0x18 */
          uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                   */
          __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register, offset: 0x20 */
          __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register, offset: 0x24 */
          uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                              */
          __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register, offset: 0x30 */
          __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register, offset: 0x34 */
          __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register, offset: 0x38 */
          uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                   */
          
          /*!< RCC APB1 peripheral clock enable register, offset: 0x40 */
          __IO uint32_t APB1ENR;       
          
          /*!< RCC APB2 peripheral clock enable register, offset: 0x44 */
          __IO uint32_t APB2ENR;       
          
          uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                              */
          
          /*!< RCC AHB1 peripheral low power mode register, offset: 0x50 */
          __IO uint32_t AHB1LPENR;     
          
          /*!< RCC AHB2 peripheral low power mode register, offset: 0x54 */
          __IO uint32_t AHB2LPENR;     
          
          /*!< RCC AHB3 peripheral low power mode register, offset: 0x58 */
          __IO uint32_t AHB3LPENR;     
          
          uint32_t      RESERVED4;     /*!< Reserved, 0x5C */
          
          /*!< RCC APB1 peripheral low power mode register, offset: 0x60 */
          __IO uint32_t APB1LPENR;     
          
          /*!< RCC APB2 peripheral low power mode register, offset: 0x64 */
          __IO uint32_t APB2LPENR;     
          
          uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C         */
          
          __IO uint32_t BDCR;          /*!< RCC Backup domain control register,offset: 0x70 */
          
          __IO uint32_t CSR;           /*!< RCC clock control & status register,offset: 0x74 */
          
          uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C          */
          
          /*!< RCC spread spectrum clock generation register,offset: 0x80 */
          __IO uint32_t SSCGR;         
          
          /*!< RCC PLLI2S configuration register,            offset: 0x84 */
          __IO uint32_t PLLI2SCFGR;    
          
          /*!< RCC PLLSAI configuration register,            offset: 0x88 */
          __IO uint32_t PLLSAICFGR;    
          
          /*!< RCC Dedicated Clocks configuration register,  offset: 0x8C */
          __IO uint32_t DCKCFGR;       

        } RCC_TypeDef;

        #define RCC                 ((RCC_TypeDef *) RCC_BASE)


        #define LED1	1
        #define LED2  2

        void init_led(void);
        void led_on(int ledx);
        void led_off(int ledx);
        
编辑完成后，保存文件到"\基础实验\project\led_nolib\MDK\user"  文件夹中，命名为led.h文件。

同样的方式创建一个led.c源文件，保存在 \基础实验\project\led_nolib\MDK\user目录，编写代码如下：

        #include "led.h"

        void init_led(void)
        {

            RCC->AHB1ENR|=1<<0;              //使能PORTA时钟 
            RCC->AHB1ENR|=1<<1;              //使能PORTB时钟 

            GPIOA_Reg->	MODER&=~(3<<(8*2));  //先清除 PA8 原来的设置
            GPIOA_Reg->	MODER|=1<<(8*2);     //设置 PA8 为输出模式
            GPIOA_Reg->	OTYPER&=~(1<<8) ;    //清除 PA8 原来的设置
            GPIOA_Reg->	OTYPER|=0<<8;      	 //设置 PA8 为推挽输出
            GPIOA_Reg-> OSPEEDR&=~(3<<(8*2));//先清除 PA8 原来的设置
            GPIOA_Reg-> OSPEEDR|=3<<(8*2);   //设置 PA8 输出速度为 100Mhz 
            GPIOA_Reg-> PUPDR&=~(3<<(8*2));  //先清除 PA8 原来的设置
            GPIOA_Reg-> PUPDR|=0<<(8*2);     //设置 PA8 不带上下拉
            GPIOA_Reg->	ODR|=1<<8;        	 //设置 PA8 输出 1（高电平,LED1初始为关闭状态）

            GPIOB_Reg->	MODER&=~(3<<(8*2));  //先清除 PB8 原来的设置
            GPIOB_Reg->	MODER|=1<<(8*2);     //设置 PB8 为输出模式
            GPIOB_Reg->	OTYPER&=~(1<<8) ;    //清除 PB8 原来的设置
            GPIOB_Reg->	OTYPER|=0<<8;      	 //设置 PB8 为推挽输出
            GPIOB_Reg-> OSPEEDR&=~(3<<(8*2));//先清除 PB8 原来的设置
            GPIOB_Reg-> OSPEEDR|=3<<(8*2);   //设置 PB8 输出速度为 100Mhz 
            GPIOB_Reg-> PUPDR&=~(3<<(8*2));  //先清除 PB8 原来的设置
            GPIOB_Reg-> PUPDR|=0<<(8*2);     //设置 PB8 不带上下拉
            GPIOB_Reg->	ODR|=1<<8;        	 //设置 PB8 输出 1（高电平,LED1初始为关闭状态）
        }


        void led_on(int ledx)
        {

            if(LED1==ledx)
            {
                GPIOA_Reg->	ODR &=~(1<<8);   //设置 PA8 输出 0（低电平,LED1亮）

            }
            else if(LED2==ledx)
            {
                GPIOB_Reg->	ODR &=~(1<<8);   //设置 PB8 输出 0（低电平,LED2亮）

            }
            else
            {
                ;							//ledx 不存在处理
            }

        }

        void led_off(int ledx)
        {

            if(LED1==ledx)
            {
                GPIOA_Reg->	ODR|=1<<8;     //设置 PA8 输出 1（高电平,LED1关闭）

            }
            else if(LED2==ledx)
            {
                GPIOB_Reg->	ODR|=1<<8;     //设置 PB8 输出 1（高电平,LED2关闭）

            }
            else
            {
                ;							//ledx 不存在处理
            }
        }


同样的方式创建一个main.c源文件，在main函数中调用刚刚写的led驱动程序，保存在 \基础实验\project\led_nolib\MDK\user 目录，编写代码如下：

        #include "led.h"

        void mydelay(int delaytime);

        int main(void)
        {
            init_led();

            while(1)
            {
                led_on(LED1);
                led_off(LED2);

                mydelay(1000);

                led_off(LED1);
                led_on(LED2);

                mydelay(1000);
            }
        }

        void mydelay(int delaytime)
        {
            int i;

            for(;delaytime>0;delaytime--)
                        for(i=0;i<1000;i++)
                                ;

        } 


复制MDK的stm32F407启动文件  \STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm\startup_stm32f40xx.s 到\基础实验\project\led_nolib\MDK\user目录下，并取消文件只读权限。好了，所有文件都已经到位，我们开始将文件加入到我们刚刚新的工程，如下图：

![](img/chapter06/1.3.27.png) 

这个工程主要为了说明，直接寄存器操作方式，所以这里我们先不使用库函数的对系统做初始化，而是使用默认系统初始化值，因此我们需要修改startup_stm32f40_41xxx.s文件，双击工程窗口的文件startup_stm32f40_41xxx.s，打开文件，先将Ｓystem_Init函数注释掉，修改如下：

![](img/chapter06/1.3.28.png) 

修改保存后，开始通过usb线将开发板连接PC，设置

![](img/chapter06/1.3.29.png) 
 
 
![](img/chapter06/1.3.30.png) 
 
 
![](img/chapter06/1.3.31.png) 

 
编译调试代码,并察看结果
 
 
![](img/chapter06/1.3.32.png) 


## 位带区功能，驱动代码设计 ##

在STM32F4xx 器件中，外设寄存器和SRAM 均映射到一个位段区域，这样可实现单个位段的读写操作。这些操作仅适用于Cortex™-M4F 访问，对于其它总线主接口（如DMA）无效。可通过一个映射公式说明别名区域中的每个字与位段区域中各个位之间的对应关系。映射公式为：

        bit_word_addr= bit_band_base+ (byte_offsetx 32) + (bit_number× 4)
        
其中：

 * — bit_word_addr 代表别名区域中将映射到目标位的字的地址
 * — bit_band_base 代表别名区域的起始地址
 * — byte_offset 代表目标位所在位段区域中的字节编号
 * — bit_number 代表目标位的位位置(0-7)

**示例**
    
下例说明如何将SRAM 地址0x20000300 处字节的位2 映射到别名区域：

        0x22006008 = 0x22000000 + (0x300*32) + (2*4)
        
对地址0x22006008 执行写操作相当于在SRAM 地址0x20000300 处字节的位2 执行读-修改-写操作。

那么在我们实验中如何使用位带区功能呢，在led.h文件中添加如下代码:

        #define BIT_BAND_ENB   

        #ifdef	BIT_BAND_ENB

        #define  BITBAND(addr,  bitnum)  ((addr  &  0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
        #define  BIT_ADDR(addr, bitnum)   *((volatile unsigned long   *)(BITBAND(addr, bitnum))) 

        #define PAout(n)  BIT_ADDR((unsigned int)(&(GPIOA_Reg->ODR)),n)//PA 输出位带区地址
        #define PBout(n)  BIT_ADDR((unsigned int)(&(GPIOB_Reg->ODR)),n)//PB 输出位带区地址 

        #endif

修改led.c代码如下：

        void led_on(int ledx)
        {
        #ifdef	BIT_BAND_ENB	
            if(LED1==ledx)
            {
                PAout(8)=0;        			//设置 PA8 输出 0（高电平,LED1亮）

            }
            else if(LED2==ledx)
            {
                PBout(8)=0;;        		//设置 PB8 输出 0（低电平,LED2亮）		

            }
            else
            {
                ;							//ledx 参数错误处理
            }

        #else

            if(LED1==ledx)
            {
                GPIOA_Reg->	ODR &=~(1<<8); //设置 PA8 输出 0（低电平,LED1亮）							
            }
            else if(LED2==ledx)
            {
                GPIOB_Reg->	ODR &=~(1<<8); //设置 PB8 输出 0（低电平,LED2亮）

            }
            else
            {
                ;							//ledx 参数错误处理
            }


        #endif
        }

        void led_off(int ledx)
        {
        #ifdef	BIT_BAND_ENB	

            if(LED1==ledx)
            {
                PAout(8)=1;        			//设置 PBA 输出 1（高电平,LED1关闭）		

            }
            else if(LED2==ledx)
            {
                PBout(8)=1;        			//设置 PB8 输出 1（高电平,LED2关闭）			

            }
            else
            {
                ;							//ledx参数错误处理
            }

            #else

            if(LED1==ledx)
            {
                GPIOA_Reg->	ODR|=1<<8;     //设置 PA8 输出 1（高电平,LED1关闭）	

            }
            else if(LED2==ledx)
            {
                GPIOB_Reg->	ODR|=1<<8;     //设置 PB8 输出 1（高电平,LED2关闭）	

            }
            else
            {
                ;						  //ledx参数错误处理
            }


        #endif

        }   

代码编辑完成，编译调试代码,并查看结果

## 使用固件库驱动代码设计 ##

上面是我们在讲解固件库之前会首先对寄存器对GPIO操作进行一个讲解，这样的话大家对驱动硬件有个初步的了解(实际上就是在读写相应的寄存器达到对硬件的控制)，对以后的学习也很有帮助。在编写上面的代码的时候，我们发现在使用的时候，每次都直接操作寄存器配置IO，代码会比较多，也不容易记住，另外，用库函数实现的好处是在各个 STM32 芯片上面的移植性非常好，不需要修改任何代码，所以我们在讲解寄存器的同时会讲解是用库函数配置 IO的方法。

### 库函数和数据结构 ###

本案例涉及的库函数函及相关定义在文件stm32f4xx_gpio.h,stm32f4xx_rcc.h和 stm32f4xx_gpio.c, stm32f4xx_rcc.c中。
    
    结构体:
        typedef struct
        {
            /*This parameter can be any value of @ref GPIO_pins_define */
            uint32_t GPIO_Pin;              
            
            /*!< Specifies the operating mode for the selected pins.*/
            GPIOMode_TypeDef GPIO_Mode;     
            
            /*!< Specifies the speed for the selected pins.*/
            GPIOSpeed_TypeDef GPIO_Speed;   
            
            /*!the operating output type for the selected pins.*/
            GPIOOType_TypeDef GPIO_OType;   
            
            /*!< Specifies the operating Pull-up/Pull down  pins.*/
            GPIOPuPd_TypeDef GPIO_PuPd;     
        }GPIO_InitTypeDef;
        
GPIO_InitTypeDef结构体定义了GPIO管脚的大部分属性包括组内编号，输入输出模式,速度，输出类型，上下拉。
            
库函数:

        void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
        
外设时钟使能函数，GPIO是挂载在 AHB1 时钟下,所以要使用RCC_AHB1PeriphClockCmd这个函数,挂在非AHB1时钟的外设不能使用该函数进行使能。 第一个参数为外设名称，可以在stm32f4xx_rcc.h文件中找到 RCC_AHB1Periph外设的定义。第二个参数为枚举类型，其定义如下：

        typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
            
        void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
        
这个函数主要实现 GPIO 的初始化，第一个参数为GPIOx组寄存器的起始地址，第二个参数为已经初始化的结构体GPIO_InitTypeDef的地址。

        void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)；
        
设置ODR 寄存器 输出管脚为高电平，第一个参数为GPIOx组寄存器的起始地址，第二个参数为组内GPIO编号。
        
        void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
        
清ODR 寄存器 输出管脚为低电平，第一个参数为GPIOx组寄存器的起始地址，第二个参数为组内GPIO编号。
        
            
### 软件设计、开发 ###

首先我们复制我们的第一个工程firstPrj文件夹，重命名为led，修改工程文件名为led.uvproj。通常情况下双击工程文件便可以打开这个新的工程，在菜单-->Project-->option --> Output选项卡 中 配置Name of Executable 为led。

drv文件组中，我们只需要保留，stm32f4xx_gpio.c，stm32f4xx_rcc.c两个文件即可。也可以都编译，只不过其他编译了也不会链接，浪费时间。

新建led.h led.c源文件分别保存在 \基础实验\bsp4Elink\inc\, \基础实验\bsp4Elink\src\ 目录中，以后我们用库函数写的版级驱动程序(bsp)都放在bsp4Elink这个目录中，以后其他实验或工程需要使用到该驱动只需要include 头文件即可，复制led_nolib工程中的main.c 到led\user\ 目录。最终我们建立的工程目录结构如下：

![](img/chapter06/1.3.33.png) 

(1) 编辑led.h，代码如下：

        #ifndef __LED_H
        #define __LED_H 
        
        #define LED1 1
        #define LED2 2

        void init_led(void);
        void led_on(int ledx);
        void led_off(int ledx);
        
        #endif

(2) 编辑led.c，代码如下：

        #include "led.h"
        #include "stm32f4xx.h"

        void init_led(void)
        {
            GPIO_InitTypeDef  GPIO_InitStructure;

            /* Enable the GPIO_LED Clock */
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

            /* Configure the GPIO_LED pin */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            GPIO_SetBits(GPIOA,GPIO_Pin_8);       //熄灭LED1
            GPIO_SetBits(GPIOB,GPIO_Pin_8);       //熄灭LED2

        }

        void led_on(int ledx)
        {
            if(LED1==ledx)
                GPIO_ResetBits(GPIOA,GPIO_Pin_8); //点亮LED1 
            if(LED2==ledx)
                GPIO_ResetBits(GPIOB,GPIO_Pin_8); //点亮LED2
        }

        void led_off(int ledx)
        {
            if(LED1==ledx)
                GPIO_SetBits(GPIOA,GPIO_Pin_8);   //熄灭LED1 
            if(LED2==ledx)
                GPIO_SetBits(GPIOB,GPIO_Pin_8);   //熄灭LED2

        }

(3) 编写延时函数实现开关状态延时保持。

    delay.h
        #ifndef __DELAY_H
        #define __DELAY_H 
        
        void mydelay(int delaytime);
        
        #endif
        
    delay.c    
    
        void mydelay(int delaytime)
        {
	       int i;
	
	       for(;delaytime>0;delaytime--)
				for(i=0;i<1000;i++)
						;
		
        }  

(4) 编写main函数进行测试

        #include "led.h"
        #include "delay.h"

        int main(void)
        {
            init_led();

            while(1)
            {
                led_on(LED1);
                led_off(LED2);

                mydelay(1000);

                led_off(LED1);
                led_on(LED2);

                mydelay(1000);
            }
        }


### 下载测试 ###

代码编辑完成，编译调试代码,并查看结果
