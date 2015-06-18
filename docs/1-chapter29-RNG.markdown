# 随机数实验 #

按K1键来获取硬件随机数，并且将获取到的随机数值打印出来

## 实验原理与基础知识 ##

STM32F4 自带了硬件随机数发生器RNG 是一个以连续模拟噪声为基础的32位随机数发生器。随机数发生器框图

![](img/chapter29/1.1.1.png) 

**RNG 控制寄存器(RNG_CR)       RNG control register**
偏移地址：0x00
    
![](img/chapter29/1.1.2.png) 

**RNG 状态寄存器(RNG_SR)       RNG status register**
偏移地址：0x04
    
![](img/chapter29/1.1.3.png) 
    
**RNG 数据寄存器(RNG_DR)       RNG data register**
偏移地址：0x08

RNG_DR 寄存器是只读寄存器，在读取时提供32 位随机数值。读取后，此寄存器在最多40 个PLL48CLK 时钟周期后，提供新的随机数值。在读取RNDATA 值之前，软件必须检查DRDY 位是否已置1。

## 电路设计 ##

## 库函数和数据结构 ##
相关的操作在文件 stm32f4xx_rng.c 和对应的头文件 stm32f4xx_rng.h 中

        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
        
AHB2 总线外设时钟使能函数,随机数发生器时钟来自 PLL48CK，通过AHB2ENR 寄存器使能。

        RNG_Cmd(ENABLE);
        
使能随机数发生器函数 将RNG_CR 寄存器的最低位设置为 1，使能随机数发生器。

        FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
        
获取随机数发生器状态的函数,每次读取随机数值之前，必须先判断 RNG_SR寄存器的 DRDY 位，如果该位为 1，则可以读取 RNG_DR 得到随机数值，如果不为 1，则需要等待。

        uint32_t RNG_GetRandomNumber(void);
        
读取随机数发生器产生的随机数函数。

## 软件设计、开发 ##
在工程中添加rng.c 文件和rng.h 文件，所有 RNG 相关的驱动代码和定义都在这两个文件中。

初始化硬件随机数发生器

        int init_rng(void)
        {
            u16 retry=0; 
            RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);  //开启 RNG 时钟   
            RNG_Cmd(ENABLE);  //使能 RNG
            while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)//等待就绪
            {  
                retry++; 
                delay_ms(1); 
            }
            if(retry>=10000)return 1;//随机数产生器工作不正常
            return 0;
        }
        
编写读取随机数值函数

        u32 get_RandomNum(void)
        { 
            while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);  //等待随机数就绪
            return RNG_GetRandomNumber(); 
        }

硬件随机数发生器产生随机数是32位数，我们通过取模的方式实现产生随机数的范围

        int get_RandomRange(int min,int max)
        { 
            return get_RandomNum()%(max-min+1) +min;
        }

编写main函数进行测试

        if(button_value==button_read())
		{
			switch(button_value)
			{
				case 1:
				    led_on(LED1);
					break;
					
				case 2:
					led_on(LED2);
					printf("random number %d\r\n", get_RandomRange(0,0xFF));	
					break;
             }
         }
         
## 软件下载、测试验证 ##


