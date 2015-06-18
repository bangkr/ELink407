# 时钟系统 #

众所周知，时钟系统是 CPU 的脉搏，就像人的心跳一样。所以时钟系统的重要性就不言而喻了。  STM32F4 的时钟系统比较复杂，不像简单的 51 单片机一个系统时钟就可以解决一切。于是有人要问，采用一个系统时钟不是很简单吗？为什么 STM32 要有多个时钟源呢？  因为首先 STM32 本身非常复杂，外设非常的多，但是并不是所有外设都需要系统时钟这么高的频率，比如看门狗以及 RTC 只需要几十 k 的时钟即可。同一个电路，时钟越快功耗越大，同时抗电磁干扰能力也会越弱，所以对于较为复杂的 MCU 一般都是采取多时钟源的方法来解决这些问题。

## STM32F407 时钟系统 ##

首先让我们来看看 STM32F407 的时钟系统图： 

![](img/chapter05/1.1.1.png) 

在 STM32F4 中，有 5 个最重要的时钟源，为 HSI、HSE、LSI、LSE、PLL。其中 PLL 实际是分为两个时钟源，分别为主 PLL 和专用 PLL。 从时钟频率来分可以分为高速时钟源和低速时钟源，在这 5 个中 HSI，HSE 以及 PLL 是高速时钟，LSI 和 LSE 是低速时钟。从来源可分为外部时钟源和内部时钟源，外部时钟源就是从外部通过接晶振的方式获取时钟源，其中 HSE 和LSE 是外部时钟源，其他的是内部时钟源,我们按图中红圈的顺序讲解。    

1、LSI 是低速内部时钟， RC 振荡器，频率为 32kHz 左右, 供独立看门狗和自动唤醒单元使用。   

2、LSE 是低速外部时钟，接频率为 32.768kHz 的石英晶体。这个主要是 RTC 的时钟源。   

3、HSE 是高速外部时钟，可接石英/陶瓷谐振器，或者接外部时钟源，频率范围为 4MHz~26MHz。ELink407接的是 8M 的晶振。HSE 也可以直接做为系统时钟或者 PLL 输入。   

4、HSI 是高速内部时钟，RC 振荡器，频率为 16MHz。可以直接作为系统时钟或者用作 PLL输入。    

5、PLL 为锁相环倍频输出。STM32F4 有两个 PLL:    

 * 1）  主 PLL(PLL)由 HSE 或者 HSI 提供时钟信号，并具有两个不同的输出时钟。   

    * 第一个输出 PLLP 用于生成高速的系统时钟（最高 168MHz）   

    * 第二个输出 PLLQ 用于生成 USB OTG FS 的时钟（48MHz），随机数发生器的时钟和 SDIO时钟。   

 * 2）专用 PLL(PLLI2S)用于生成精确时钟，从而在 I2S 接口实现高品质音频性能。   

从图可以看出，SYSCLK 系统时钟来源有三个方面：  HSI,HSE 和 PLL。在我们实际应用中，因为对时钟速度要求都比较高所以一般情况下，都是采用 PLL (PLLCLK)作为 SYSCLK时钟源。HSI或者HSE输入到一个分频系数为 M 的分频器，然后经过倍频系数为 N 的倍频器出来之后的时候还需要经过一个分频系数为 P（第一个输出 PLLP）或者 Q（第二个输出 PLLQ）的分频器分频之后，最后才生成最终的主 PLL 时钟。   

例如我们的外部晶振选择 8MHz，设置HSE 为 PLL 时钟源。同时我们设置相应的分频器 M=8，倍频器倍频系数 N=336，分频器分频系数 P=2，那么主 PLL 生成的第一个输出高速时钟 PLLP 为：   

        PLL=8MHz * N/ (M*P)=8MHz* 336 /(8*2) = 168MHz
        
如果设置的 SYSCLK 时钟源为 PLL，那么 SYSCLK 时钟为  168MHz。后面的实验都是采用这样的配置。  

设置了内核的SYSCLK 时钟源，我们再看各个外设以及系统提供时钟，这里我们选择一些比较常用的时钟知识来讲解。从上到下的顺序进行说明。

看门狗时钟源。从图中可以看出，看门狗时钟源只能是低速的 LSI 时钟。   

RTC 时钟源，从图上可以看出，RTC 的时钟源可以选择 LSI，LSE，以及HSE 分频后的时钟，HSE 分频系数为 2~31。  

STM32F4 输出时钟 MCO1 和 MCO2。 MCO1 是向芯片的 PA8 引脚输出时钟。它有四个时钟来源分别为：HSI,LSE,HSE 和 PLL 时钟。MCO2 是向芯片的PC9 输出时钟，它同样有四个时钟来源分别为： HSE,PLL， SYSCLK 以及 PLLI2S时钟。MCO 输出时钟频率最大不超过 100MHz。    

以太网 PTP 时钟， AHB 时钟， APB2 高速时钟， APB1 低速时钟。这些时钟都是来源于 SYSCLK 系统时钟。其中以太网 PTP 时钟是使用系统时钟。AHB,APB2 和 APB1 时钟是经过 SYSCLK 时钟分频得来。这里大家记住，AHB最大时钟为 168MHz, APB2 高速时钟最大频率为 84MHz,而 APB1 低速时钟最大频率为 42MHz。  

I2S 时钟源。从图可以看出，I2S 的时钟源来源于 PLLI2S 或者映射到 I2S_CKIN 引脚的外部时钟。 I2S 出于音质的考虑，对时钟精度要求很高。   

STM32F4 内部以太网 MAC 时钟的来源。对于 MII 接口来说，必须向外部PHY 芯片提供 25Mhz 的时钟，这个时钟，可以由 PHY 芯片外接晶振，或者使用STM32F4 的 MCO 输 出 来 提 供 。 然 后 ， PHY 芯 片 再 给 STM32F4 提供ETH_MII_TX_CLK 和 ETH_MII_RX_CLK 时钟。对于 RMII 接口来说，外部必须提供 50Mhz 的时钟驱动 PHY 和 STM32F4 的 ETH_RMII_REF_CLK，这个 50Mhz时钟可以来自 PHY、有源晶振或者 STM32F4 的 MCO。我们的开发板使用的是RMII 接 口 ， 使 用 PHY 芯 片 提 供 50Mhz 时 钟 驱 动 STM32F4 的ETH_RMII_REF_CLK。   

外部 PHY 提供的 USB OTG HS（60MHZ）时钟。这里还需要说明一下，Cortex 系统定时器 Systick 的时钟源可以是 AHB 时钟 HCLK 或HCLK 的 8 分频。

在以上的时钟输出中，有很多是带使能控制的，例如 AHB 总线时钟、内核时钟、各种 APB1外设、APB2 外设等等。当需要使用某模块时，记得一定要先使能对应的时钟。后面我们讲解实例的时候会讲解到时钟使能的方法。  


## RCC相关库函数说明 (STM32F4 时钟初始化配置)##

上一小节我们对 STM32F4 时钟树进行了初步的讲解，接下来我们来讲解一下 STM32F4 的系统时钟配置。

STM32F4 时钟系统初始化是在 system_stm32f4xx.c 中的 SystemInit()函数中完成的。 对于系统时钟关键寄存器设置主要是在 SystemInit 函数中调用 SetSysClock()函数来设置的。我们可以先看看 SystemInit ()函数体：

        void SystemInit(void)
        {
            /* FPU settings */
            #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
            SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));   /* set CP10 and CP11 Full Access */
            #endif 
            
            /* Reset the RCC clock configuration to the default reset state */
            /* Set HSION bit */
            RCC->CR |= (uint32_t)0x00000001;
            
            /* Reset CFGR register */
            RCC->CFGR = 0x00000000;
            
            /* Reset HSEON, CSSON and PLLON bits */
            RCC->CR &= (uint32_t)0xFEF6FFFF;
            
            /* Reset PLLCFGR register */
            RCC->PLLCFGR = 0x24003010;
            
            /* Reset HSEBYP bit */
            RCC->CR &= (uint32_t)0xFFFBFFFF;
            
            /* Disable all interrupts */
            RCC->CIR = 0x00000000;
            
            #if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
            SystemInit_ExtMemCtl(); 
            #endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */
            
            /* Configure the System clock source, PLL Multiplier and Divider factors, 
            AHB/APBx prescalers and Flash settings */
            SetSysClock();
            
            /* Configure the Vector Table location add offset address */
            #ifdef VECT_TAB_SRAM
            /* Vector Table Relocation in Internal SRAM */
            SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
            #else
            SCB->VTOR  =  FLASH_BASE  |  VECT_TAB_OFFSET;  /*  Vector  Table  Relocation  in
            Internal FLASH */
            #endif
        }
        
SystemInit 函数开始先进行浮点运算单元设置，然后是复位 PLLCFGR,CFGR 寄存器，同时通过设置 CR 寄存器的 HSI 时钟使能位来打开 HSI 时钟。默认情况下如果 CFGR 寄存器复位，那么是选择 HSI 作为系统时钟，这点大家可以查看 RCC->CFGR 寄存器的位描述最低 2 位可以得知，当低两位配置为 00 的时候（复位之后），会选择 HSI 振荡器为系统时钟。也就是说，调用 SystemInit 函数之后，首先是选择 HSI 作为系统时钟。下面是 RCC->CFGR 寄存器配置描述：     

![](img/chapter05/1.1.2.png) 

**位31:30**  MCO2[1:0]：微控制器时钟输出2 (Microcontroller clock output 2)由软件置1 和清零。时钟源选择可能会造成对MCO2 的干扰。强烈建议仅在复位后但在使能外部振荡器和PLL 之前来配置这些位。

 * 00：选择系统时钟(SYSCLK) 输出到MCO2 引脚   
 * 01:选择PLLI2S 时钟输出到MCO2 引脚   
 * 10：选择HSE 振荡器时钟输出到MCO2 引脚    
 * 11：选择PLL 时钟输出到MCO2 引脚   


**位27:29**  MCO2PRE：MCO2 预分频器(MCO2 prescaler)由软件置1 和清零，用于配置MCO2 的预分频器。对此预分频器进行修改可能会对 M C O 2 造成干扰。强烈建议仅在复位后且在使能外部振荡器和PLL 之前进行此分频器的更改。

 * 0xx：无分频   
 * 100：2 分频  
 * 101：3 分频    
 * 110：4 分频    
 * 111：5 分频  


**位24:26**  MCO1PRE：MCO1 预分频器(MCO1 prescaler)由软件置1 和清零，用于配置MCO1 的预分频器。对此预分频器进行修改可能会对 M C O 1 造成干扰。强烈建议仅在复位后且在使能外部振荡器和PLL 之前进行此分频器的更改。

 * 0xx：无分频   
 * 100：2 分频   
 * 101：3 分频   
 * 110：4 分频   
 * 111：5 分频  

**位23**     I2SSRC：I2S 时钟选择(I2S clock selection)由软件置1 和清零。通过此位可在PLLI2S 时钟和外部时钟之间选择I2S 时钟源。强烈建议仅在复位之后使能I2S 模块之前对此位进行更改。

 * 0：PLLI2S 时钟用作I2S 时钟源  
 * 1：在I2S_CKIN 引脚上映射的外部时钟用作I2S 时钟源   


**位22:21**  MCO1：微控制器时钟输出1 (Microcontroller clock output 1)由软件置1 和清零。时钟源选择可能会造成对MCO1 的干扰。强烈建议仅在复位后且在使能 外部振荡器和PLL 之前来配置这些位。
 * 00：选择HSI 时钟输出到MCO1 引脚   
 * 01：选择LSE 振荡器输出到MCO1 引脚   
 * 10：选择HSE 振荡器时钟输出到MCO1 引脚   
 * 11：选择PLL 时钟输出到MCO1 引脚  


**位20:16**  RTCPRE：适用于RTC 时钟的HSE 分频系数由软件置1 和清零，用于对HSE 时钟输入时钟进行分频，进而为RTC 生成1 MHz 的时钟。

 * 00000：无时钟   
 * 00001：无时钟  
 * 00010：HSE/2  
 * 00011：HSE/3  
 * 00100：HSE/4  
 * ...  
 * 11110：HSE/30  
 * 11111：HSE/31   


**位15:13**  PPRE2：APB 高速预分频器(APB2) (APB high-speed prescaler (APB2))由软件置位和清零，用于控制APB 高速时钟分频系数。

 * 0xx：AHB 时钟不分频   
 * 100：AHB 时钟2 分频   
 * 101：AHB 时钟4 分频   
 * 110：AHB 时钟8 分频   
 * 111：AHB 时钟16 分频  


**位12:10**  PPRE1：APB 低速预分频器(APB1) (APB Low speed prescaler (APB1))由软件置位和清零，用于控制APB 低速时钟分频系数。

 * 0xx：AHB 时钟不分频   
 * 100：AHB 时钟2 分频  
 * 101：AHB 时钟4 分频   
 * 110：AHB 时钟8 分频   
 * 111：AHB 时钟16 分频  


**位9:8**    保留，必须保持复位值。


**位7:4**    HPRE：AHB 预分频器(AHB prescaler)由软件置位和清零，用于控制AHB 时钟分频系数。

 * 0xxx：系统时钟不分频   
 * 1000：系统时钟2 分频   
 * 1001：系统时钟4 分频   
 * 1010：系统时钟8 分频    
 * 1011：系统时钟16 分频   
 * 1100：系统时钟64 分频    
 * 1101：系统时钟128 分频   
 * 1110：系统时钟256 分频   
 * 1111：系统时钟512 分频  


**位3:2**    SWS：系统时钟切换状态(System clock switch status)由硬件置1 和清零，用于指示用作系统时钟的时钟源。

 * 00：HSI 振荡器用作系统时钟   
 * 01：HSE 振荡器用作系统时钟   
 * 10：PLL 用作系统时钟   
 * 11：不适用  


**位1:0**    SW：系统时钟切换(System clock switch)由软件置1 和清零，用于选择系统时钟源。由硬件置1，用于在退出停机或待机模式时或者在直接或间接用作系统时钟的HSE 振荡器发生故障时强制HSI 的选择。
 * 00：选择HSI 振荡器作为系统时钟   
 * 01：选择HSE 振荡器作为系统时钟   
 * 10：选择PLL 作为系统时钟     
 * 11：不允许   
        
        
在设置完相关寄存器后，接下来 SystemInit 函数内部会调用 SetSysClock 函数。这个函数比较长，我们就把函数一些关键代码行截取出来给大家讲解一下。这里我们省略一些宏定义标识符值的判断而直接把针对 STM32F407 比较重要的内容贴出来：   

        static void SetSysClock(void)
        {
            __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
            
            /*使能 HSE*/
            RCC->CR |= ((uint32_t)RCC_CR_HSEON);
            
            /*  等待 HSE 稳定*/
            do
            {
                HSEStatus = RCC->CR & RCC_CR_HSERDY;
                StartUpCounter++;
            } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));
            
            if ((RCC->CR & RCC_CR_HSERDY) != RESET)
            {
                HSEStatus = (uint32_t)0x01;
            }
            else
            {
                HSEStatus = (uint32_t)0x00;
            }
            
            if (HSEStatus == (uint32_t)0x01)
            {
                /* Select regulator voltage output Scale 1 mode */
                RCC->APB1ENR |= RCC_APB1ENR_PWREN;
                PWR->CR |= PWR_CR_VOS;
                
                /* HCLK = SYSCLK / 1*/
                RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
                
                /* PCLK2 = HCLK / 2*/ 
                RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
                
                /* PCLK1 = HCLK / 4*/
                RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
                
                /* PCLK2 = HCLK / 2*/
                RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
                
                /* PCLK1 = HCLK / 4*/
                RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
                
                /* Configure the main PLL */
                RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
                
                /*  使能主 PLL*/
                RCC->CR |= RCC_CR_PLLON;
                
                /*  等待主 PLL 就绪  */
                while((RCC->CR & RCC_CR_PLLRDY) == 0)
                {
                }
                
                /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
                FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN 
                |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;
                
                /*  设置主 PLL 时钟为系统时钟源  */
                RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
                RCC->CFGR |= RCC_CFGR_SW_PLL;
                
                /*  等待设置稳定（主 PLL 作为系统时钟源）*/
                while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
                {
                }
                
            }
            else
            { 
                /* If HSE fails to start-up, the application will have wrong clock
                configuration. User can add here some code to deal with this error */
            }
        }
        
这段代码的大致流程是这样的：先使能外部时钟 HSE ，等待 HSE 稳定之后，配置AHB, APB1, APB2  时钟相关的分频因子，也就是相关外设的时钟。等待这些都配置完成之后，打开主 PLL 时钟，然后设置主 PLL 作为系统时钟 SYSCLK 时钟源。如果 HSE 不能达到就绪状态（比如外部晶振不能稳定或者没有外部晶振），那么依然会是 HSI 作为系统时钟。在这里要特别提出来，在设置主 PLL 时钟的时候，会要设置一系列的分频系数和倍频系数参数。大家可以从 SetSysClock 函数的这行代码看出：

        RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
        (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
        
        
        
## 修改系统时钟的方法 ##
        
这些参数是通过宏定义标识符的值来设置的。默认的配置在 System_stm32f4xx.c 文件开头的地方配置。对于我们开发板，我们的设置参数值如下：

        #define PLL_M       8
        #define PLL_Q       7
        #define PLL_N       336
        #define PLL_P       2
        
        
因此我们修改System_stm32f4xx.c代码为：
        /********** PLL Parameters *******/
        #if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || \
        defined(STM32F429_439xx) || defined(STM32F401xx)
        /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
        #define PLL_M      8//25
        #elif defined (STM32F446xx)
        #define PLL_M      8
        #elif defined (STM32F411xE)

        #if defined(USE_HSE_BYPASS)
        #define PLL_M      8    
        #else  
        #define PLL_M      16
        #endif /* USE_HSE_BYPASS */

        #endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx */  

        /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
        #define PLL_Q      7

        #if defined(STM32F446xx)
        /* PLL division factor for I2S, SAI, SYSTEM and SPDIF: Clock =  PLL_VCO / PLLR */
        #define PLL_R      7
        #endif /* STM32F446xx */ 

        #if defined(STM32F40_41xxx) || defined(STM32F427_437xx) ||  \
        defined(STM32F429_439xx) || defined(STM32F446xx)
        #define PLL_N      336//360
        /* SYSCLK = PLL_VCO / PLL_P */
        #define PLL_P      2
        #endif /* STM32F40_41xxx || STM32F427_437x || STM32F429_439xx || STM32F446xx */

所以我们的主 PLL 时钟为：  

        PLL=8MHz * N/ (M*P)=8MHz* 336 /(8*2) = 168MHz
        
在开发过程中，我们可以通过调整这些值来设置我们的系统时钟。这里还有个特别需要注意的地方，就是我们还要同步修改 stm32f4xx.h 中宏定义标识符HSE_VALUE 的值为我们的外部时钟：

        #if !defined   (HSE_VALUE) 
        
        /*!< Value of the External oscillator in Hz */
        #define HSE_VALUE     ((uint32_t)8000000) 
        
        #endif /* HSE_VALUE */
        
这里默认固件库配置的是 25000000，我们外部时钟为 8MHz，所以我们根据我们硬件情况修改为 8000000 即可。

讲到这里，大家对 SystemInit 函数的流程会有个比较清晰的理解。那么 SystemInit 函数是怎么被系统调用的呢？SystemInit 是整个设置系统时钟的入口函数。这个函数对于我们使用 ST提供的 STM32F4 固件库的话，会在系统启动之后先执行 main 函数，然后再接着执行 SystemInit函数实现系统相关时钟的设置。这个过程设置是在启动文件 startup_stm32f40_41xxx.s 中间设置的，我们接下来看看启动文件中这段启动代码：

        ; Reset handler
        Reset_Handler     PROC
        EXPORT   Reset_Handler              [WEAK]
        IMPORT   SystemInit
        IMPORT   __main
        LDR      R0, =SystemInit
        BLX      R0
        LDR      R0, =__main
        BX       R0
        ENDP
        
这段代码的作用是在系统复位之后引导进入 main 函数，同时在进入 main 函数之前，首先要调用 SystemInit 系统初始化函数完成系统时钟等相关配置。 

最后我们总结一下 SystemInit()函数中设置的系统时钟大小：

        SYSCLK（系统时钟）                 =168MHz
        AHB 总线时钟(HCLK=SYSCLK)          =168MHz
        APB1 总线时钟(PCLK1=SYSCLK/4)      =42MHz
        APB2 总线时钟(PCLK2=SYSCLK/2)      =84MHz
        PLL 主时钟                         =168MHz
        
## RCC相关库函数说明 (STM32F4 时钟使能和配置) ##

上小节我们讲解了系统复位之后调用 SystemInit 函数之后相关时钟的默认配置。如果在系统初始化之后，我们还需要修改某些时钟源配置，或者我们要使能相关外设的时钟该怎么设置呢？这些设置实际是在 RCC 相关寄存器中配置的。因为 RCC 相关寄存器非常多，有兴趣的同学可以直接打开《STM32F4 中文参考手册》 6.3 小节查看所有 RCC 相关寄存器的配置。所以这里我们不直接讲解寄存器配置，而是通过 STM32F4 标准固件库配置方法给大家讲解。

在 STM32F4 标准固件库里，时钟源的选择以及时钟使能等函数都是在 RCC 相关固件库文件 stm32f4xx_rcc.h 和 stm32f4xx_rcc.c 中声明和定义的。大家打开 stm32f4xx_rcc.h 文件可以看到文件开头有很多宏定义标识符，然后是一系列时钟配置和时钟使能函数申明。这些函数大致可以归结为三类，一类是外设时钟使能函数，一类是时钟源和分频因子配置函数，还有一类是外设复位函数。当然还有几个获取时钟源配置的函数。下面我们以几种常见的操作来简要介绍一下这些库函数的使用。
首先是时钟使能函数。时钟使能相关函数包括外设设置使能和时钟源使能两类。首先我们来看看外设时钟使能相关的函数：  

        void    RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
        void    RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
        void    RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
        void    RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
        void    RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
        
这里主要有 5 个外设时钟使能函数。5 个函数分别用来使能 5 个总线下面挂载的外设时钟，这些总线分别为：AHB1 总线，AHB2 总线，AHB3 总线，APB1 总线以及 APB2 总线。要使能某个外设，调用对应的总线外设时钟使能函数即可。这里我们要特别说明一下， STM32F4 的外设在使用之前，必须对时钟进行使能，如果没有使能时钟，那么外设是无法正常工作的。对于哪个外设是挂载在哪个总线之下，虽然我们也可以查手册查询到，但是这里如果大家使用的是库函数的话，实际上是没有必要去查询手册的，这里我们给大家介绍一个小技巧。

比如我们要使能 GPIOA,我们只需要在 stm32f4xx_rcc.h 头文件里面搜索 GPIOA，就可以搜索到对应的时钟使能函数的第一个入口参数为 RCC_AHB1Periph_GPIOA，从这个宏定义标识符一眼就可以看出，GPIOA 是挂载在 AHB1 下面。同理，对于串口 1 我们可以搜索 USART1，找到标识符为 RCC_APB2Periph_USART1，那么很容易知道串口 1 是挂载在 APB2 之下。

如果我们要使能 GPIOA，那么我们可以在头文件 stm32f4xx_rcc.h 里面查看到宏定义标识符 RCC_AHB1Periph_GPIOA，顾名思义 GPIOA 是挂载在 AHB1 总线之下，所以，我们调用AHB1 总线下外设时钟使能函数 RCC_AHB1PeriphClockCmd 即可。具体调用方式入如下：

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能 GPIOA 时钟 
        
同理，如果我们要使能串口 1 的时钟，那么我们调用的函数为：

        void   RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
具体的调用方法是：

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
        
还有一类时钟使能函数是时钟源使能函数，前面我们已经讲解过 STM32F4 有 5 大类时钟源。这里我们列出来几种重要的时钟源使能函数：

        void   RCC_HSICmd(FunctionalState NewState);
        void   RCC_LSICmd(FunctionalState NewState);
        void   RCC_PLLCmd(FunctionalState NewState);
        void   RCC_PLLI2SCmd(FunctionalState NewState);
        void   RCC_PLLSAICmd(FunctionalState NewState);
        void   RCC_RTCCLKCmd(FunctionalState NewState);
        
这些函数是用来使能相应的时钟源。比如我们要使能 PLL 时钟，那么调用的函数为：

        void   RCC_PLLCmd(FunctionalState NewState);
        
具体调用方法如下：

        RCC_PLLCmd(ENABLE);
        
我们要使能相应的时钟源，调用对应的函数即可。

接下来我们要讲解的是第二类时钟功能函数：时钟源选择和分频因子配置函数。这些函数是用来选择相应的时钟源以及配置相应的时钟分频系数。比如我们之前讲解过系统时钟SYSCLK，我们可以选择 HSI,HSE 以及 PLL 三个中的一个时钟源为系统时钟。那么到底选择哪一个，这是可以配置的。下面我们列举几种时钟源配置函数：

        void       RCC_LSEConfig(uint8_t RCC_LSE);
        void       RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
        void       RCC_HCLKConfig(uint32_t RCC_SYSCLK);
        void       RCC_PCLK1Config(uint32_t RCC_HCLK);
        void       RCC_PCLK2Config(uint32_t RCC_HCLK);
        void       RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
        void       RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t PLLM, 
        uint32_t PLLN, uint32_t PLLP, uint32_t PLLQ);
        
比如我们要设置系统时钟源为 HSI，那么我们可以调用系统时钟源配置函数：  

        void   RCC_HCLKConfig(uint32_t RCC_SYSCLK);
        
具体配置方法如下：

        RCC_HCLKConfig(RCC_SYSCLKSource_HSI);//配置时钟源为 HSI
        
又如我们要设置 APB1 总线时钟为 HCLK 的 2 分频，也就是设置分频因子为 2 分频，那么如果我们要使能 HSI，那么调用的函数为：

        void    RCC_PCLK1Config(uint32_t RCC_HCLK);
        
具体配置方法如下：

        RCC_PCLK1Config(RCC_HCLK_Div2);
        
接下来我们看看第三类外设复位函数。如下：

        void    RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
        void    RCC_AHB2PeriphResetCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
        void    RCC_AHB3PeriphResetCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState);
        void    RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); 
        void    RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
        
这类函数跟前面讲解的外设时钟函数使用方法基本一致，不同的是一个是用来使能外设时钟，一个是用来复位对应的外设。这里大家在调用函数的时候一定不要混淆。对于这些时钟操作函数，我们就不一一列举出来，大家可以打开 RCC 对应的文件仔细了解。
