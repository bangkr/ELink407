# 软件开发环境 #

## STM32软件开发基本知识 ##

### STM32编程基础 ###

在学习前，用户必须掌握c语言语法、变量空间、程序空间、堆栈、c编译器和调试器工作原理等相关知识，也必须了解CPU内部Flash和内部RAM的特性和地址空间。为了方便广大初学者学习，我们对这些概念进行一些必要的说明。 

首先，我们必须明确设计程序的最终目的是生成一个可用于烧写的文件，这个文件可以通过烧写软件烧写到CPU内部Flash。在CPU上电后，CPU的硬件取指系统会自动的读取并执行Flash中的指令序列。 

“源文件“是指由汇编源文件(*.s)和c源文件（*.c、*.h）构成的文件集合。我们通过编辑器（比如UltraEdit、SourceInsight或者IDE开发环境自带的编辑器）来书写源文件。源文件经过编译器（*.s由汇编编译器负责编译，*.c、*.h由c编译器负责编译）编译后生成中间目标文件(*.obj)。每个源文件会对应一个同名的obj文件，这些obj文件中的程序地址以及变量地址是未定位的。之后，由连接器负责将这些*.obj文件链接为一个完整的目标文件（如axf文件），这个axf文件包含CPU可执行的二进制代码和一些必要的调试信息。生成axf文件后，我们就可以通过集成开发环境（IDE）提供的调试接口下载程序到Flash，并通过仿真器进行跟踪调试。为了便于产品批量烧写程序，我们可以借助于IDE提供的工具，将axf文件转换为可直接烧写的文件（如hex文件）。 

“程序空间“指二进制可执行指令序列（包括一些常量）存放的空间，”变量空间“指程序运行时各种变量（局部的和全局的）以及堆栈的存放空间。程序空间可以定位在CPU内部Flash、CPU内部RAM、外部SRAM和外部NOR Flash。变量空间只能定位在CPU内部SRAM和外部SRAM。 

在使用STM32开发产品时，程序空间必须定位在CPU内部Flash，因为上电后，CPU都是从内部Flash开始取值运行的。在调试开发阶段，为了节省程序装载时间和延长Flash寿命，可以将代码空间定位到CPU内部RAM或者外部SRAM。

在c程序中，栈（stack）用来保存函数返回地址、局部变量空间分配。堆（heap）用于通过malloc之类函数给变量分配空间。栈和堆的大小需要根据用户的具体程序进行合理设置，过小的栈和堆会引起程序执行异常。 

### C语言 ###

C 语言知识博大精深，也不是我们三言两语能讲解清楚，同时我们相信学 STM32F4 这种级别 MCU 的用户，C 语言基础应该都是没问题的。我们这里主要是简单的复习一下几个 C 语言基础知识点，引导那些 C 语言基础知识不是很扎实的用户能够快速开发 STM32 程序。同时希望这些用户能够多去复习一下 C 语言基础知识，C 语言毕竟是单片机开发中的必备基础知识。对于 C 语言基础比较扎实的用户，这部分知识可以忽略不看。

#### 位操作 ####

C 语言位操作相信学过 C 语言的人都不陌生了,简而言之，就是对基本类型变量可以在位级别进行操作。这节的内容很多朋友都应该很熟练了，我这里也就点到为止，不深入探讨。下面我们先讲解几种位操作符，然后讲解位操作使用技巧。

C 语言支持如下 6 种位操作

      | 运算符  | 含义     | 运算符 | 含义    |
      |  &      | 按位与   |  ~     |  取反   |
      |  |      | 按位或   |  <<    |  左移   |
      |  ^      | 按位异或 |  >>    |  右移   |

这些与或非，取反，异或，右移，左移这些到底怎么回事，这里我们就不多做详细，相信大家学 C 语言的时候都学习过了。如果不懂的话，可以百度一下，非常多的知识讲解这些操作符。下面我们想着重讲解位操作在单片机开发中的一些实用技巧。

**1) 不改变其他位的值的状况下，对某几个位进行设值**

这个场景单片机开发中经常使用，方法就是先对需要设置的位用&操作符进行清零操作，然后用|操作符设值。比如我要改变 GPIOA-> BSRRL 的状态,可以先对寄存器的值进行&清零操作       
    
        GPIOA-> BSRRL &=0XFF0F;  //将第 4-7 位清 0然后再与需要设置的值进行|或运算
        GPIOA-> BSRRL |=0X0040;  //设置相应位的值，不改变其他位的值      

**2) 移位操作提高代码的可读性。**  
移位操作在单片机开发中也非常重要，我们来看看下面一行代码    
      
        GPIOx->ODR = (((uint32_t)0x01) << pinpos); 

这个操作就是将 ODR 寄存器的第 pinpos 位设置为 1，为什么要通过左移而不是直接设置一个固定的值呢？其实，这是为了提高代码的可读性以及可重用性。这行代码可以很直观明了的知道，是将第 pinpos 位设置为 1。如果你写成  
   
        GPIOx->ODR =0x0030;

这样的代码就不好看也不好重用了。

**3) ~取反操作使用技巧**   
SR 寄存器的每一位都代表一个状态，某个时刻我们希望去设置某一位的值为 0，同时其他位都保留为 1，简单的作法是直接给寄存器设置一个值：

        TIMx->SR=0xFFF7；

这样的作法设置第 3 位为 0，但是这样的作法同样不好看，并且可读性很差。看看库函数代码中怎样使用的：

        TIMx->SR = (uint16_t)~TIM_FLAG;    

而 TIM_FLAG 是通过宏定义定义的值：

        #define TIM_FLAG_Update                     ((uint16_t)0x0001)    
        #define TIM_FLAG_CC1                        ((uint16_t)0x0002)

看这个应该很容易明白，可以直接从宏定义中看出 TIM_FLAG_Update 就是设置的第 0 位了，可读性非常强。

#### struct ####

经常很多用户提到，他们对结构体使用不是很熟悉，但是 MDK 中太多地方使用结构体以及结构体指针，这让他们一下子摸不着头脑，学习 STM32 的积极性大大降低，其实结构体并不是那么复杂，这里我们稍微提一下结构体的一些知识，还有一些知识我们会在下一节的“寄存器地址名称映射分析”中讲到一些。声明结构体类型：

        Struct  结构体名{  
            成员列表;  
            }变量名列表；  

例如： 

        Struct U_TYPE {  
            int BaudRate;  
            int   WordLength;   
        }usart1,usart2;

在结构体申明的时候可以定义变量，也可以申明之后定义，方法是：

        Struct  结构体名字  结构体变量列表  ;

例如：struct U_TYPE usart1,usart2;

结构体成员变量的引用方法是：

        结构体变量名字.成员名

比如要引用 usart1 的成员 BaudRate，方法是：usart1.BaudRate;结构体指针变量定义也是一样的，跟其他变量没有啥区别。

例如：struct U_TYPE *usart1；//定义结构体指针变量 usart1;

结构体指针成员变量引用方法是通过“->”符号实现，比如要访问 usart3 结构体指针指向的结构体的成员变量 BaudRate,方法是：

        usart3->BaudRate;

上面讲解了结构体和结构体指针的一些知识，其他的什么初始化这里就不多讲解了。 讲到这里，有人会问，结构体到底有什么作用呢？为什么要使用结构体呢？下面我们将简单的通过一个实例回答一下这个问题。

在我们单片机程序开发过程中，经常会遇到要初始化一个外设比如串口，它的初始化状态是由几个属性来决定的，比如串口号，波特率，极性，以及模式等。对于这种情况，在我们没有学习结构体的时候，我们一般的方法是：

        void USART_Init(u8 usartx,u32 u32 BaudRate,u8 parity,u8 mode);

这种方式是有效的同时在一定场合是可取的。但是试想，如果有一天，我们希望往这个函数里面再传入一个参数，那么势必我们需要修改这个函数的定义，重新加入字长这个入口参数。于是我们的定义被修改为：

        void USART_Init (u8 usartx,u32 BaudRate, u8 parity,u8 mode,u8 wordlength );

但是如果我们这个函数的入口参数是随着开发不断的增多，那么是不是我们就要不断的修改函数的定义呢？这是不是给我们开发带来很多的麻烦？那又怎样解决这种情况呢？

这样如果我们使用到结构体就能解决这个问题了。我们可以在不改变入口参数的情况下，只需要改变结构体的成员变量，就可以达到上面改变入口参数的目的。结构体就是将多个变量组合为一个有机的整体。上面的函数，BaudRate,wordlength,Parity,mode,wordlength 这些参数，他们对于串口而言，是一个有机整体，都是来设置串口参数的，所以我们可以将他们通过定义一个结构体来组合在一个。MDK 中是这样定义的：

        typedef struct   
        {   
            uint32_t USART_BaudRate;   
            uint16_t USART_WordLength;  
            uint16_t USART_StopBits;   
            uint16_t USART_Parity;  
            uint16_t USART_Mode;   
            uint16_t USART_HardwareFlowControl;    
        } USART_InitTypeDef;  

于是，我们在初始化串口的时候入口参数就可以是 USART_InitTypeDef 类型的变量或者指针变量了，MDK 中是这样做的：

        void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);

这样，任何时候，我们只需要修改结构体成员变量，往结构体中间加入新的成员变量，而不需要修改函数定义就可以达到修改入口参数同样的目的了。这样的好处是不用修改任何函数定义就可以达到增加变量的目的。

理解了结构体在这个例子中间的作用吗？在以后的开发过程中，如果你的变量定义过多，如果某几个变量是用来描述某一个对象，你可以考虑将这些变量定义在结构体中，这样也许可以提高你的代码的可读性。

使用结构体组合参数，可以提高代码的可读性，不会觉得变量定义混乱。当然结构体的作用就远远不止这个了，同时，MDK 中用结构体来定义外设也不仅仅只是这个作用，这里我们只是举一个例子，通过最常用的场景，让大家理解结构体的一个作用而已。后面一节我们还会讲解结构体的一些其他知识。

#### define ####

define 是 C 语言中的预处理命令，它用于宏定义，可以提高源代码的可读性，为编程提供方便。常见的格式：

        #define  标识符  字符串

“标识符”为所定义的宏名。“字符串”可以是常数、表达式、格式串等。例如：

        #define PLL_M       8

定义标识符 PLL_M 的值为 8。至于 define 宏定义的其他一些知识，比如宏定义带参数这里我们就不多讲解。


#### ifdef 条件编译 #####

单片机程序开发过程中，经常会遇到一种情况，当满足某条件时对一组语句进行编译，而当条件不满足时则编译另一组语句。条件编译命令最常见的形式为： 

        #ifdef  标识符   
            程序段 1   
        #else   
            程序段 2   
        #endif   
    
它的作用是：当标识符已经被定义过(一般是用#define 命令定义)，则对程序段 1 进行编译 否则编译程序段 2。 其中#else 部分也可以没有，即： 

        #ifdef   
            程序段 1   
        #endif   

这个条件编译在 MDK 里面是用得很多的，在 stm32f4xx.h 这个头文件中经常会看到这样的语句：

        #if defined (STM32F40_41xxx)  
            STM32F40x 系列和 STM32F41x 系列芯片需要的一些变量定义  
        #end  

而(STM32F40_41xxx 则是我们通过#define 来定义的。条件编译也是 c 语言的基础知识。

#### typedef ####  

typedef 用于为现有类型创建一个新的名字，或称为类型别名，用来简化变量的定义。typedef 在 MDK 用得最多的就是定义结构体的类型别名和枚举类型了。 

        struct _GPIO  
        {  
            __IO uint32_t MODER;  
            __IO uint32_t OTYPER;   
        };  

定义了一个结构体 GPIO，这样我们定义变量的方式为：

        struct    _GPIO    GPIOA;//定义结构体变量 GPIOA

但是这样很繁琐，MDK 中有很多这样的结构体变量需要定义。这里我们可以为结体定义一个别名 GPIO_TypeDef，这样我们就可以在其他地方通过别名 GPIO_TypeDef 来定义结构体变量了。方法如下：

        typedef struct  
        {  
            __IO uint32_t MODER;   
            __IO uint32_t OTYPER;   
            …   
        } GPIO_TypeDef;

Typedef 为结构体定义一个别名 GPIO_TypeDef，这样我们可以通过 GPIO_TypeDef 来定义结构体变量：

        GPIO_TypeDef _GPIOA,_GPIOB;

这里的 GPIO_TypeDef 就跟 struct _GPIO 是等同的作用了。 这样是不是方便很多？ 

#### extern ####

C 语言中 extern 可以置于变量或者函数前，以表示变量或者函数的定义在别的文件中，提示编译器遇到此变量和函数时在其他模块中寻找其定义。 这里面要注意，对于 extern 申明变量可以多次，但定义只有一次。在我们的代码中你会看到看到这样的语句：

        extern u16 USART_RX_STA;  

这个语句是申明 USART_RX_STA 变量在其他文件中已经定义了，在这里要使用到。所以，你肯定可以找到在某个地方有变量定义的语句：

        u16 USART_RX_STA; 

下面通过一个例子说明一下使用方法。

在 Main.c 定义的全局变量 id，id 的初始化都是在 Main.c 里面进行的。

        **Main.c 文件**

        u8 id;//定义只允许一次  
        main()  
        {  
            id=1;   
            printf("d%",id);//id=1  
            test();  
            printf("d%",id);//id=2  
        }  

但是我们希望在 main.c 的  changeId(void)函数中使用变量 id,这个时候我们就需要在 main.c里面去申明变量 id 是外部定义的了，因为如果不申明，变量 id 的作用域是到不了 main.c 文件中。看下面 main.c 中的代码：

        extern u8 id;//申明变量 id 是在外部定义的，申明可以在很多个文件中进行  
        void test(void){  
            id=2;  
        }

在 main.c 中申明变量 id 在外部定义，然后在 main.c 中就可以使用变量 id 了。对于 extern 申明函数在外部定义的应用，这里我们就不多讲解了。
    
#### static ####

static 修饰用来限定符号的作用域，分为修饰变量和修饰函数之分。

static 修饰函数其作用域限于只在当前源文件中调用该函数，另外static 函数在内存中只有一份，普通函数在每个被调用中维持一份拷贝。

static修饰变量可分为修饰全局变量和局部变量之分，static 局部变量只被初始化一次，下一次依据上一次结果值；static 全局变量只初使化一次，跟修饰函数一样，其作用域限于当前源文件，可以防止在其他文件单元中被引用，修改。

#### volatile ####

一个定义为 volatile 的变量是说这变量可能会被意想不到地改变，这样，编译器就不会去假设这个变量的值了。精确地说就是， 优化器在用到这个变量时必须每次都小心地重新读取这个变量的值， 而不是使用保存在寄存器里的备份。
下面是 volatile 变量的几个例子：

1). 并行设备的硬件寄存器（如：状态寄存器）

2). 一个中断服务子程序中会访问到的非自动变量(Non-automatic variables) 

3). 多线程应用中被几个任务共享的变量
 
## ST 固件库 ##

ST(意法半导体)为了方便用户开发程序，提供了一套丰富的 STM32F4 固件库。到底什么是固件库？它与直接操作寄存器开发有什么区别和联系？很多初学用户很是费解，这一节，我们将讲解 STM32 固件库相关的基础知识，希望能够让大家对 STM32F4 固件库有一个初步的了解

所谓的固件库就是将一组功能相关的特殊功能寄存器的操作过程封装为一些C语言函数，以方便应用程序调用，方便使用者记忆。其实固件库中的每一个函数，如果单步跟踪进去，就是原生的寄存器操作代码。也许你看不懂这种寄存器操作形式，不过没关系，我们后面将举个例子来介绍如何阅读这些代码。我在这个地方需要表达的是，直接操作寄存器和调用固件库中函数本质上没有区别，不会因为就多封装了1层就改变了性质。

### 如何获得开发资源 ##
固件库包光盘路径（是压缩包形式，大家解压即可） ：

        STM32_Lib\stm32f4_dsp_stdperiph_lib.zip
        
同时，大家也可以到ST官网 http://www.st.com [下载](http://www.st.com/web/catalog/tools/FM147/CL1794/SC961/SS1743/LN1939/PF257901)

### 固件库开发与寄存器开发 ###

我们所有的例程都是用ST官方固件库进行开发的，全部是C语言。 很多从51过来的初学者很排斥用固件库，认为固件库太复杂，看不懂，找个寄存器定义都找不到；用库学不到东西。如果你持有这种观点，只能说明你c语言没有过关。还停留在一个  main.c  文件就是全部程序的最初级阶段。   

在 51 的开发中我们常常的作法是直接操作寄存器，比如要控制某些 IO 口的状态，我们直接操作寄存器：

        P0=0x11;
        
而在 STM32 的开发中，我们同样可以操作寄存器：

        GPIOF->BSRRL=0x0001; //这里是针对 STM32F4 系列
        
这种方法当然可以，但是这种方法的劣势是你需要去掌握每个寄存器的用法，你才能正确使用STM32，而对于 STM32 这种级别的 MCU，数百个寄存器记起来又是谈何容易。于是 ST(意法半导体)推出了官方固件库，固件库将这些寄存器底层操作都封装起来，提供一整套接口（API）供开发者调用，大多数场合下，你不需要去知道操作的是哪个寄存器，你只需要知道调用哪些函数即可。  

比如上面的控制 BSRRL 寄存器实现电平控制，官方库封装了一个函数：

        void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
        {
            GPIOx->BSRRL = GPIO_Pin;
        } 
        
这个时候你不需要再直接去操作 BSRRL 寄存器了，你只需要知道怎么使用 GPIO_SetBits  ()这个函数就可以了。在你对外设的工作原理有一定的了解之后，你再去看固件库函数，基本上函数名字能告诉你这个函数的功能是什么，该怎么使用，这样是不是开发会方便很多？  

任何处理器，不管它有多么的高级，归根结底都是要对处理器的寄存器进行操作。但是固件库不是万能的，您如果想要把 STM32 学透，光读 STM32 固件库是远远不够的。你还是要了解一下 STM32 的原理，了解 STM32 各个外设的运行机制。只有了解了这些原理，你在进行固件库开发过程中才可能得心应手游刃有余。 只有了解了原理，你才能做到“知其然知其所以然”，所以大家在学习库函数的同时，要了解一下寄存器大致配置过程。

### 固件库与CMSIS标准 ###

STM32F4 固件库就是函数的集合，那么对这些函数有什么要求呢？？这里就涉及到一个 CMSIS 标准的基础知识。 经常有人问到 STM32 和 ARM 以及 ARM7 是什么关系这样的问题，其实 ARM 是一个做芯片标准的公司，它负责的是芯片内核的架构设计，而 TI，ST 这样的公司，他们并不做标准，他们是芯片公司，他们是根据 ARM 公司提供的芯片内核标准设计自己的芯片。所以，任何一个做 Cortex-M4 芯片，他们的内核结构都是一样的，不同的是他们的存储器容量，片上外设，IO 以及其他模块的区别。所以你会发现，不同公司设计的Cortex-M4 芯片他们的端口数量，串口数量，控制方法这些都是有区别的，这些资源他们可以根据自己的需求理念来设计。同一家公司设计的多种 Cortex-M4 内核芯片的片上外设也会有很大的区别，比如 STM32F407 和 STM32F429，他们的片上外设就有很大的区别。

既然大家都使用的是 Cortex-M4 核，也就是说，本质上大家都是一样的，这样 ARM 公司为了能让不同的芯片公司生产的 Cortex-M4 芯片能在软件上基本兼容，和芯片生产商共同提出了一套标准 CMSIS 标准(Cortex  Microcontroller  Software  Interface  Standard )  ,翻译过来是“ARM Cortex™  微控制器软件接口标准”。 ST 官方库就是根据这套标准设计的。这里我们又要引用参考资料里面的图片来看看基于 CMSIS 应用程序基本结构如下图

        图 3 基于 CMSIS 应用程序基本结构
CMSIS 分为 3 个基本功能层：  
1)  核内外设访问层：ARM 公司提供的访问，定义处理器内部寄存器地址以及功能函数。   
2)  中间件访问层:定义访问中间件的通用 API。由 ARM 提供，芯片厂商根据需要更新。  
3)  外设访问层：定义硬件寄存器的地址以及外设的访问函数。   
从图中可以看出， CMSIS 层在整个系统中是处于中间层，向下负责与内核和各个外设直接打交道，向上提供实时操作系统用户程序调用的函数接口。如果没有 CMSIS 标准，那么各个芯片公司就会设计自己喜欢的风格的库函数，而 CMSIS 标准就是要强制规定，芯片生产公司设计的库函数必须按照 CMSIS 这套规范来设计。
其实不用这么讲这么复杂的，一个简单的例子，我们在使用 STM32 芯片的时候首先要进行系统初始化， CMSIS 规范就规定，系统初始化函数名字必须为 SystemInit，所以各个芯片公司写自己的库函数的时候就必须用 SystemInit 对系统进行初始化。CMSIS 还对各个外设驱动文件的文件名 字规范化，以及函数名字规范化等等一系列规定。上一节讲的函数GPIO_ResetBits 这个函数名字也是不能随便定义的，是要遵循 CMSIS 规范的。


### 官方固件库安装 ###
这一节内容主要讲解 ST 官方提供的 STM32F4 固件库包的结构 。固件库是不断完善升级的，所以有不同的版本，我们使用的是 V1.5 版本的固件库,大家可以到以下目录找到其压缩文件:

        \STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib.zip
        
然后解压即可。 该压缩包是我们从ST官方[下载](http://www.st.com/web/catalog/tools/FM147/CL1794/SC961/SS1743/LN1939/PF257901)的，目前最新版V1.5 ，感兴趣的同学也可去检查是否有升级版下载。下面看看官方库包的目录结构，如下图所示：  

Libraries  
├─CMSIS  
│  │  CMSIS_END_USER_LICENCE_AGREEMENT.pdf  
│  │  CMSIS_END_USER_LICENCE_AGREEMENT.rtf  
│  │  index.html   
│  │  README.txt   
│  │    
│  ├─Device  
│  │  └─ST   
│  │      └─STM32F4xx  
│  │          │  Release_Notes.html  
│  │          │    
│  │          ├─Include  
│  │          │      stm32f4xx.h  
│  │          │      system_stm32f4xx.h  
│  │          │         
│  │          └─Source   
│  │              └─Templates   
│  │                  │  system_stm32f4xx.c   
│  │                  │     
│  │                  ├─arm   
│  │                  │      startup_stm32f401xx.s  
│  │                  │      startup_stm32f40xx.s  
│  │                  │      startup_stm32f40_41xxx.s   
│  │                  │      startup_stm32f411xe.s   
│  │                  │      startup_stm32f427x.s  
│  │                  │      startup_stm32f427_437xx.s  
│  │                  │      startup_stm32f429_439xx.s  
│  │                  │      startup_stm32f446xx.s  
│  │                  │        
│  │                  ├─gcc_ride7  
│  │                  │      startup_stm32f401xx.s   
│  │                  │      startup_stm32f40xx.s   
│  │                  │      startup_stm32f40_41xxx.s   
│  │                  │      startup_stm32f427x.s   
│  │                  │      startup_stm32f427_437xx.s   
│  │                  │      startup_stm32f429_439xx.s   
│  │                  │        
│  │                  ├─iar   
│  │                  │      startup_stm32f401xx.s  
│  │                  │      startup_stm32f40xx.s   
│  │                  │      startup_stm32f40_41xxx.s  
│  │                  │      startup_stm32f411xe.s  
│  │                  │      startup_stm32f427x.s   
│  │                  │      startup_stm32f427_437xx.s   
│  │                  │      startup_stm32f429_439xx.s   
│  │                  │      startup_stm32f446xx.s   
│  │                  │        
│  │                  ├─TASKING   
│  │                  │      cstart_thumb2.asm   
│  │                  │         
│  │                  └─TrueSTUDIO   
│  │                          startup_stm32f401xx.s   
│  │                          startup_stm32f40xx.s   
│  │                          startup_stm32f40_41xxx.s   
│  │                          startup_stm32f411xe.s   
│  │                          startup_stm32f427xx.s   
│  │                          startup_stm32f427_437xx.s  
│  │                          startup_stm32f429_439xx.s  
│  │                          startup_stm32f446xx.s   
│  │                            
│  ├─DSP_Lib  
│  │  │  license.txt   
│  │  │     
│  │  └─Source   
│  │      ├─BasicMathFunctions   
│  │      │      arm_abs_f32.c  
│  │      │      arm_abs_q15.c   
│  │      │      arm_abs_q31.c   
│  │      │      arm_abs_q7.c   
│  │      │      arm_add_f32.c  
│  │      │      arm_add_q15.c   
│  │      │      arm_add_q31.c  
│  │      │      arm_add_q7.c  
│  │      │      arm_dot_prod_f32.c  
│  │      │      arm_dot_prod_q15.c  
│  │      │      arm_dot_prod_q31.c   
│  │      │      arm_dot_prod_q7.c  
│  │      │      arm_mult_f32.c   
│  │      │      arm_mult_q15.c   
│  │      │      arm_mult_q31.c   
│  │      │      arm_mult_q7.c   
│  │      │      arm_negate_f32.c  
│  │      │      arm_negate_q15.c   
│  │      │      arm_negate_q31.c   
│  │      │      arm_negate_q7.c    
│  │      │      arm_offset_f32.c  
│  │      │      arm_offset_q15.c  
│  │      │      arm_offset_q31.c  
│  │      │      arm_offset_q7.c   
│  │      │      arm_scale_f32.c   
│  │      │      arm_scale_q15.c   
│  │      │      arm_scale_q31.c  
│  │      │      arm_scale_q7.c  
│  │      │      arm_shift_q15.c  
│  │      │      arm_shift_q31.c  
│  │      │      arm_shift_q7.c  
│  │      │      arm_sub_f32.c   
│  │      │      arm_sub_q15.c  
│  │      │      arm_sub_q31.c   
│  │      │      arm_sub_q7.c  
│  │      │        
│  │      ├─CommonTables   
│  │      │      arm_common_tables.c  
│  │      │      arm_const_structs.c  
│  │      │        
│  │      ├─ComplexMathFunctions   
│  │      │      arm_cmplx_conj_f32.c   
│  │      │      arm_cmplx_conj_q15.c   
│  │      │      arm_cmplx_conj_q31.c   
│  │      │      arm_cmplx_dot_prod_f32.c  
│  │      │      arm_cmplx_dot_prod_q15.c  
│  │      │      arm_cmplx_dot_prod_q31.c  
│  │      │      arm_cmplx_mag_f32.c  
│  │      │      arm_cmplx_mag_q15.c   
│  │      │      arm_cmplx_mag_q31.c   
│  │      │      arm_cmplx_mag_squared_f32.c   
│  │      │      arm_cmplx_mag_squared_q15.c   
│  │      │      arm_cmplx_mag_squared_q31.c   
│  │      │      arm_cmplx_mult_cmplx_f32.c  
│  │      │      arm_cmplx_mult_cmplx_q15.c   
│  │      │      arm_cmplx_mult_cmplx_q31.c  
│  │      │      arm_cmplx_mult_real_f32.c  
│  │      │      arm_cmplx_mult_real_q15.c  
│  │      │      arm_cmplx_mult_real_q31.c   
│  │      │        
│  │      ├─ControllerFunctions   
│  │      │      arm_pid_init_f32.c   
│  │      │      arm_pid_init_q15.c   
│  │      │      arm_pid_init_q31.c  
│  │      │      arm_pid_reset_f32.c  
│  │      │      arm_pid_reset_q15.c   
│  │      │      arm_pid_reset_q31.c   
│  │      │      arm_sin_cos_f32.c   
│  │      │      arm_sin_cos_q31.c  
│  │      │        
│  │      ├─FastMathFunctions   
│  │      │      arm_cos_f32.c   
│  │      │      arm_cos_q15.c   
│  │      │      arm_cos_q31.c   
│  │      │      arm_sin_f32.c  
│  │      │      arm_sin_q15.c  
│  │      │      arm_sin_q31.c   
│  │      │      arm_sqrt_q15.c   
│  │      │      arm_sqrt_q31.c  
│  │      │        
│  │      ├─FilteringFunctions   
│  │      │      arm_biquad_cascade_df1_32x64_init_q31.c  
│  │      │      arm_biquad_cascade_df1_32x64_q31.c  
│  │      │      arm_biquad_cascade_df1_f32.c  
│  │      │      arm_biquad_cascade_df1_fast_q15.c   
│  │      │      arm_biquad_cascade_df1_fast_q31.c   
│  │      │      arm_biquad_cascade_df1_init_f32.c   
│  │      │      arm_biquad_cascade_df1_init_q15.c   
│  │      │      arm_biquad_cascade_df1_init_q31.c   
│  │      │      arm_biquad_cascade_df1_q15.c   
│  │      │      arm_biquad_cascade_df1_q31.c   
│  │      │      arm_biquad_cascade_df2T_f32.c   
│  │      │      arm_biquad_cascade_df2T_f64.c   
│  │      │      arm_biquad_cascade_df2T_init_f32.c  
│  │      │      arm_biquad_cascade_df2T_init_f64.c  
│  │      │      arm_biquad_cascade_stereo_df2T_f32.c   
│  │      │      arm_biquad_cascade_stereo_df2T_init_f32.c  
│  │      │      arm_conv_f32.c  
│  │      │      arm_conv_fast_opt_q15.c  
│  │      │      arm_conv_fast_q15.c   
│  │      │      arm_conv_fast_q31.c   
│  │      │      arm_conv_opt_q15.c   
│  │      │      arm_conv_opt_q7.c   
│  │      │      arm_conv_partial_f32.c   
│  │      │      arm_conv_partial_fast_opt_q15.c  
│  │      │      arm_conv_partial_fast_q15.c   
│  │      │      arm_conv_partial_fast_q31.c   
│  │      │      arm_conv_partial_opt_q15.c   
│  │      │      arm_conv_partial_opt_q7.c   
│  │      │      arm_conv_partial_q15.c  
│  │      │      arm_conv_partial_q31.c  
│  │      │      arm_conv_partial_q7.c   
│  │      │      arm_conv_q15.c  
│  │      │      arm_conv_q31.c  
│  │      │      arm_conv_q7.c   
│  │      │      arm_correlate_f32.c  
│  │      │      arm_correlate_fast_opt_q15.c   
│  │      │      arm_correlate_fast_q15.c   
│  │      │      arm_correlate_fast_q31.c   
│  │      │      arm_correlate_opt_q15.c  
│  │      │      arm_correlate_opt_q7.c  
│  │      │      arm_correlate_q15.c   
│  │      │      arm_correlate_q31.c   
│  │      │      arm_correlate_q7.c  
│  │      │      arm_fir_decimate_f3.c  
│  │      │      arm_fir_decimate_fast_q15.c   
│  │      │      arm_fir_decimate_fast_q31.c  
│  │      │      arm_fir_decimate_init_f32.c  
│  │      │      arm_fir_decimate_init_q15.c  
│  │      │      arm_fir_decimate_init_q31.c  
│  │      │      arm_fir_decimate_q15.c  
│  │      │      arm_fir_decimate_q31.c  
│  │      │      arm_fir_f32.c  
│  │      │      arm_fir_fast_q15.c   
│  │      │      arm_fir_fast_q31.c  
│  │      │      arm_fir_init_f32.c   
│  │      │      arm_fir_init_q15.c  
│  │      │      arm_fir_init_q31.c  
│  │      │      arm_fir_init_q7.c  
│  │      │      arm_fir_interpolate_f32.c   
│  │      │      arm_fir_interpolate_init_f32.c  
│  │      │      arm_fir_interpolate_init_q15.c   
│  │      │      arm_fir_interpolate_init_q31.c  
│  │      │      arm_fir_interpolate_q15.c  
│  │      │      arm_fir_interpolate_q31.c  
│  │      │      arm_fir_lattice_f32.c  
│  │      │      arm_fir_lattice_init_f32.c  
│  │      │      arm_fir_lattice_init_q15.c  
│  │      │      arm_fir_lattice_init_q31.c   
│  │      │      arm_fir_lattice_q15.c   
│  │      │      arm_fir_lattice_q31.c  
│  │      │      arm_fir_q15.c  
│  │      │      arm_fir_q31.c  
│  │      │      arm_fir_q7.c   
│  │      │      arm_fir_sparse_f32.c   
│  │      │      arm_fir_sparse_init_f32.c  
│  │      │      arm_fir_sparse_init_q15.c   
│  │      │      arm_fir_sparse_init_q31.c   
│  │      │      arm_fir_sparse_init_q7.c  
│  │      │      arm_fir_sparse_q15.c  
│  │      │      arm_fir_sparse_q31.c  
│  │      │      arm_fir_sparse_q7.c   
│  │      │      arm_iir_lattice_f32.c  
│  │      │      arm_iir_lattice_init_f32.c   
│  │      │      arm_iir_lattice_init_q15.c  
│  │      │      arm_iir_lattice_init_q31.c  
│  │      │      arm_iir_lattice_q15.c  
│  │      │      arm_iir_lattice_q31.c  
│  │      │      arm_lms_f32.c  
│  │      │      arm_lms_init_f32.c   
│  │      │      arm_lms_init_q15.c   
│  │      │      arm_lms_init_q31.c  
│  │      │      arm_lms_norm_f32.c  
│  │      │      arm_lms_norm_init_f32.c   
│  │      │      arm_lms_norm_init_q15.c   
│  │      │      arm_lms_norm_init_q31.c   
│  │      │      arm_lms_norm_q15.c  
│  │      │      arm_lms_norm_q31.c   
│  │      │      arm_lms_q15.c  
│  │      │      arm_lms_q31.c  
│  │      │        
│  │      ├─MatrixFunctions  
│  │      │      arm_mat_add_f32.c  
│  │      │      arm_mat_add_q15.c   
│  │      │      arm_mat_add_q31.c  
│  │      │      arm_mat_cmplx_mult_f32.c  
│  │      │      arm_mat_cmplx_mult_q15.c   
│  │      │      arm_mat_cmplx_mult_q31.c  
│  │      │      arm_mat_init_f32.c  
│  │      │      arm_mat_init_q15.c   
│  │      │      arm_mat_init_q31.c  
│  │      │      arm_mat_inverse_f32.c  
│  │      │      arm_mat_inverse_f64.c  
│  │      │      arm_mat_mult_f32.c  
│  │      │      arm_mat_mult_fast_q15.c   
│  │      │      arm_mat_mult_fast_q31.c  
│  │      │      arm_mat_mult_q15.c  
│  │      │      arm_mat_mult_q31.c   
│  │      │      arm_mat_scale_f32.c   
│  │      │      arm_mat_scale_q15.c   
│  │      │      arm_mat_scale_q31.c   
│  │      │      arm_mat_sub_f32.c   
│  │      │      arm_mat_sub_q15.c  
│  │      │      arm_mat_sub_q31.c  
│  │      │      arm_mat_trans_f32.c  
│  │      │      arm_mat_trans_q15.c  
│  │      │      arm_mat_trans_q31.c  
│  │      │        
│  │      ├─StatisticsFunctions  
│  │      │      arm_max_f32.c   
│  │      │      arm_max_q15.c  
│  │      │      arm_max_q31.c  
│  │      │      arm_max_q7.c  
│  │      │      arm_mean_f32.c  
│  │      │      arm_mean_q15.c  
│  │      │      arm_mean_q31.c  
│  │      │      arm_mean_q7.c  
│  │      │      arm_min_f32.c  
│  │      │      arm_min_q15.c  
│  │      │      arm_min_q31.c  
│  │      │      arm_min_q7.c  
│  │      │      arm_power_f32.c  
│  │      │      arm_power_q15.c  
│  │      │      arm_power_q31.c  
│  │      │      arm_power_q7.c  
│  │      │      arm_rms_f32.c  
│  │      │      arm_rms_q15.c  
│  │      │      arm_rms_q31.c  
│  │      │      arm_std_f32.c  
│  │      │      arm_std_q15.c  
│  │      │      arm_std_q31.c  
│  │      │      arm_var_f32.c  
│  │      │      arm_var_q15.c  
│  │      │      arm_var_q31.c  
│  │      │        
│  │      ├─SupportFunctions  
│  │      │      arm_copy_f32.c  
│  │      │      arm_copy_q15.c   
│  │      │      arm_copy_q31.c  
│  │      │      arm_copy_q7.c  
│  │      │      arm_fill_f32.c   
│  │      │      arm_fill_q15.c  
│  │      │      arm_fill_q31.c  
│  │      │      arm_fill_q7.c  
│  │      │      arm_float_to_q15.c  
│  │      │      arm_float_to_q31.c  
│  │      │      arm_float_to_q7.c  
│  │      │      arm_q15_to_float.c  
│  │      │      arm_q15_to_q31.c  
│  │      │      arm_q15_to_q7.c   
│  │      │      arm_q31_to_float.c  
│  │      │      arm_q31_to_q15.c  
│  │      │      arm_q31_to_q7.c  
│  │      │      arm_q7_to_float.c  
│  │      │      arm_q7_to_q15.c  
│  │      │      arm_q7_to_q31.c  
│  │      │        
│  │      └─TransformFunctions   
│  │              arm_bitreversal.c   
│  │              arm_bitreversal2.S   
│  │              arm_cfft_f32.c   
│  │              arm_cfft_q15.c   
│  │              arm_cfft_q31.c  
│  │              arm_cfft_radix2_f32.c  
│  │              arm_cfft_radix2_init_f32.c  
│  │              arm_cfft_radix2_init_q15.c  
│  │              arm_cfft_radix2_init_q31.c  
│  │              arm_cfft_radix2_q15.c  
│  │              arm_cfft_radix2_q31.c  
│  │              arm_cfft_radix4_f32.c   
│  │              arm_cfft_radix4_init_f32.c  
│  │              arm_cfft_radix4_init_q15.c  
│  │              arm_cfft_radix4_init_q31.c  
│  │              arm_cfft_radix4_q15.c  
│  │              arm_cfft_radix4_q31.c  
│  │              arm_cfft_radix8_f32.c   
│  │              arm_dct4_f32.c  
│  │              arm_dct4_init_f32.c   
│  │              arm_dct4_init_q15.c   
│  │              arm_dct4_init_q31.c   
│  │              arm_dct4_q15.c  
│  │              arm_dct4_q31.c  
│  │              arm_rfft_f32.c   
│  │              arm_rfft_fast_f32.c  
│  │              arm_rfft_fast_init_f32.c  
│  │              arm_rfft_init_f32.c   
│  │              arm_rfft_init_q15.c  
│  │              arm_rfft_init_q31.c  
│  │              arm_rfft_q15.c  
│  │              arm_rfft_q31.c  
│  │                
│  ├─Include  
│  │      arm_common_tables.h   
│  │      arm_const_structs.h   
│  │      arm_math.h   
│  │      core_cm0.h   
│  │      core_cm0plus.h  
│  │      core_cm3.h   
│  │      core_cm4.h  
│  │      core_cm7.h   
│  │      core_cmFunc.h   
│  │      core_cmInstr.h   
│  │      core_cmSimd.h   
│  │      core_sc000.h  
│  │      core_sc300.h  
│  │        
│  ├─Lib   
│  │  │  license.txt   
│  │  │     
│  │  ├─ARM   
│  │  │      arm_cortexM0b_math.lib   
│  │  │      arm_cortexM0l_math.lib   
│  │  │      arm_cortexM3b_math.lib   
│  │  │      arm_cortexM3l_math.lib   
│  │  │      arm_cortexM4bf_math.lib  
│  │  │      arm_cortexM4b_math.lib   
│  │  │      arm_cortexM4lf_math.lib  
│  │  │      arm_cortexM4l_math.lib  
│  │  │      arm_cortexM7bfdp_math.lib  
│  │  │      arm_cortexM7bfsp_math.lib   
│  │  │      arm_cortexM7b_math.lib   
│  │  │      arm_cortexM7lfdp_math.lib   
│  │  │      arm_cortexM7lfsp_math.lib   
│  │  │      arm_cortexM7l_math.lib   
│  │  │        
│  │  └─GCC   
│  └─RTOS  
│      └─Template  
│              cmsis_os.h   
│                
└─STM32F4xx_StdPeriph_Driver  
    │  Release_Notes.html   
    │    
    ├─inc   
    │      misc.h  
    │      stm32f4xx_adc.h  
    │      stm32f4xx_can.h  
    │      stm32f4xx_cec.h   
    │      stm32f4xx_crc.h   
    │      stm32f4xx_cryp.h  
    │      stm32f4xx_dac.h   
    │      stm32f4xx_dbgmcu.h  
    │      stm32f4xx_dcmi.h   
    │      stm32f4xx_dma.h   
    │      stm32f4xx_dma2d.h   
    │      stm32f4xx_exti.h   
    │      stm32f4xx_flash.h   
    │      stm32f4xx_flash_ramfunc.h   
    │      stm32f4xx_fmc.h   
    │      stm32f4xx_fmpi2c.h   
    │      stm32f4xx_fsmc.h   
    │      stm32f4xx_gpio.h  
    │      stm32f4xx_hash.h   
    │      stm32f4xx_i2c.h   
    │      stm32f4xx_iwdg.h   
    │      stm32f4xx_ltdc.h   
    │      stm32f4xx_pwr.h   
    │      stm32f4xx_qspi.h  
    │      stm32f4xx_rcc.h   
    │      stm32f4xx_rng.h   
    │      stm32f4xx_rtc.h   
    │      stm32f4xx_sai.h   
    │      stm32f4xx_sdio.h   
    │      stm32f4xx_spdifrx.h  
    │      stm32f4xx_spi.h  
    │      stm32f4xx_syscfg.h  
    │      stm32f4xx_tim.h   
    │      stm32f4xx_usart.h   
    │      stm32f4xx_wwdg.h   
    │        
    └─src   
            misc.c  
            stm32f4xx_adc.c   
            stm32f4xx_can.c   
            stm32f4xx_cec.c   
            stm32f4xx_crc.c   
            stm32f4xx_cryp.c   
            stm32f4xx_cryp_aes.c   
            stm32f4xx_cryp_des.c   
            stm32f4xx_cryp_tdes.c   
            stm32f4xx_dac.c  
            stm32f4xx_dbgmcu.c   
            stm32f4xx_dcmi.c   
            stm32f4xx_dma.c   
            stm32f4xx_dma2d.c   
            stm32f4xx_exti.c   
            stm32f4xx_flash.c   
            stm32f4xx_flash_ramfunc.c   
            stm32f4xx_fmc.c   
            stm32f4xx_fmpi2c.c   
            stm32f4xx_fsmc.c   
            stm32f4xx_gpio.c   
            stm32f4xx_hash.c  
            stm32f4xx_hash_md5.c   
            stm32f4xx_hash_sha1.c   
            stm32f4xx_i2c.c   
            stm32f4xx_iwdg.c   
            stm32f4xx_ltdc.c   
            stm32f4xx_pwr.c   
            stm32f4xx_qspi.c   
            stm32f4xx_rcc.c   
            stm32f4xx_rng.c  
            stm32f4xx_rtc.c  
            stm32f4xx_sai.c   
            stm32f4xx_sdio.c  
            stm32f4xx_spdifrx.c   
            stm32f4xx_spi.c   
            stm32f4xx_syscfg.c   
            stm32f4xx_tim.c  
            stm32f4xx_usart.c  
            stm32f4xx_wwdg.c   


### 官方固件库使用 ###

**文件目录介绍：**   

*  ◆ Libraries 文件夹下面有 CMSIS 和 STM32F4xx_StdPeriph_Driver 两个目录，这两个目录包含固件库核心的所有子文件夹和文件。CMSIS 文件夹存放的是符合 CMSIS 规范的一些文件。包括 STM32F4 核内外设访问层代码，DSP 软件库，RTOS API，以及 STM32F4 片上外设访问层代码等。我们新建工程的时候会添加文件到我们工程，需要注意的是该目录下的文件设定为只读权限，非必要的情况下不要修改里面的源文件，以免影响其他项目工程。    

*  ◆ STM32F4xx_StdPeriph_Driver 放的是 STM32F4 标准外设固件库源码文件和对应的头文件。  

*  ◆ inc 目录存放的是 stm32f4xx_ppp.h 头文件,无需改动。 src 目录下面放的是 stm32f4xx_ppp.c 格式的固件库源码文件。每一个.c 文件和一个相应的.h 文件对应。这里的文件也是固件库外设的关键文件，每个外设对应一组文件。  

*  ◆ Libraries 文件夹里面的文件在我们建立工程的时候都会使用到。  

*  ◆ Project 文件夹下面有两个文件夹。顾名思义， STM32F4xx_StdPeriph_Examples 文件夹下面存放的的 ST 官方提供的固件实例源码，在以后的开发过程中，可以参考修改这个官方提供的实例来快速驱动自己的外设，很多开发板的实例都参考了官方提供的例程源码，这些源码对以后的学习非常重要。  

*  ◆ STM32F4xx_StdPeriph_Template 文件夹下面存放的是工程模板。

*  ◆ Utilities 文件下就是官方评估板的一些对应源码，这个对于本手册学习可以忽略不看。根目录中还有一个 stm32f4xx_dsp_stdperiph_lib_um.chm 文件，直接打开可以知道，这是一个固件库的帮助文档，这个文档非常有用，只可惜是英文的，在开发过程中，这个文档会经常被使用到。

**关键文件介绍：**

在介绍一些关键文件之前，首先我们来看看一个基于固件库的 STM32F4 工程需要哪些关键文件，这些文件之间有哪些关联关系。其实这个可以从 ST 提供的英文版的 STM32F4 固件库说明里面找到。   

*  ◆ core_cm4.h 文件位于\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Include 目录下面的，这个就是 CMSIS 核心文件，提供进入 M4 内核接口，这是 ARM 公司提供，对所有CM4 内核的芯片都一样。  

*  ◆ stm32f4xx.h 和 system_stm32f4xx.h 文件存放在文件夹\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Include 下面。    

*  ◆ system_stm32f4xx.h 是片上外设接入层系统头文件。主要是申明设置系统及总线时钟相关的函数。与其对应的源文件 system_stm32f4xx.c 在目录\STM32F4xx_DSP_StdPeriph_Lib\Project\STM32F4xx_StdPeriph_Templates 可以找到。这个里面有一个非常重要的 SystemInit()函数申明，这个函数在我们系统启动的时候都会调用，用来设置系统的整个系统和总线时钟。    

*  ◆ stm32f4xx.h 是 STM32F4 片上外设访问层头文件。这个文件就相当重要了，只要你做STM32F4 开发，你几乎时刻都要查看这个文件相关的定义。这个文件打开可以看到，里面非常多的结构体以及宏定义。这个文件里面主要是系统寄存器定义申明以及包装内存操作，对于这里是怎样申明以及怎样将内存操作封装起来的。  

*  ◆ stm32f4xx_it.c,stm32f4xx_it.h 以及 stm32f4xx_conf.h 等 文 件 ， 我 们 可 以 从\STM32F4xx_DSP_StdPeriph_Lib\Project\STM32F4xx_StdPeriph_Templates 文件夹中找到。这几个文件我们后面新建工程也有用到。stm32f4xx_it.c 和 stm32f4xx_it.h 里面是用来编写中断服务函数，中断服务函数也可以随意编写在工程里面的任意一个文件里面   

*  ◆ stm32f4xx_conf.h 是外设驱动配置文件。 文件打开可以看到一堆的#include,这里你建立工程的时候，可以注释掉一些你不用的外设头文件。   

*  ◆ 对于上图中的 misc.c,misc.h,stm32f4xx_ppp.c,stm32f4xx_ppp.h 以及 stm32f4xx_rcc.c 和stm32f4xx_rcc.h 文件，这些文件存放在目录 Libraries\STM32F4xx_StdPeriph_Driver。这些文件是 STM32F4 标准的外设库文件。其中 misc.c 和 misc.h 是定义中断优先级分组以及 Systick 定时器相关的函数。    

*  ◆ stm32f3xx_rcc.c 和 stm32f4xx_rcc.h 是与 RCC 相关的一些操作函数，作用主要是一些时钟的配置和使能。在任何一个 STM32 工程 RCC 相关的源文件和头文件是必须添加的。    

*  ◆ 对于文件 stm32f4xx_ppp.c 和 stm32f4xx_ppp.h，这就是 stm32F4 标准外设固件库对应的源文件和头文件。包括一些常用外设 GPIO,ADC,USART 等。

*  ◆ 文件 Application.c 实际就是说是应用层代码。这个文件名称可以任意取了。我们工程中，直接取名为 main.c.实际上一个完整的 STM32F4 的工程光有上面这些文件还是不够的。还缺少非常关键的启动文件。STM32F4 的启动文件存放在目录\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm 下面。对于不同型号的 STM32F4 系列对应的启动文件也不一样。我们的开发板是 STM32F407 系列所以我们选择的启动文件为startup_stm32f40_41xxx.s。  启动文件到底什么作用，其实我们可以打开启动文件进去看看。启动文件主要是进行堆栈之类的初始化，中断向量表以及中断函数定义。启动文件要引导进入main 函数。 Reset_Handler 中断函数是唯一实现了的中断处理函数，其他的中断函数基本都是死循环。   Reset_handler 在我们系统启动的时候会调用，下面让我们看看 Reset_handler 这段代码：

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
        
这段代码的作用是在系统复位之后引导进入 main 函数，同时在进入 main 函数之前，首先要调用 SystemInit 系统初始化函数。

## ARM MDK开发工具的安装与使用 ##

RealView MDK开发套件源自德国Keil公司（后被ARM公司收购），是ARM公司目前最新推出的针对各种嵌入式处理器的软件开发工具。RealView MDK集成了业内最领先的技术，包括μVision5v集成开发环境与  RealView编译器。支持ARM7、ARM9和最新的Cortex-M3,Cortex-M3核处理器，自动配置启动代码，集成Flash烧写模块，强大的Simulation设备模拟，性能分析等功能，与ARM之前的工具包ADS等相比，RealView编译器的最新版本可将性能改善超过20％。

### ARM MDK安装 ###

1、 在ELink407\tools\arm mdk目录下找到MDK_510.exe软件，双击mdk_510.exe应用程序，进入安装界面。依次选择“next”->“勾选同意协议，next”->“选择安装目录，next“->输入姓名、公司名、email信息(任意填写)->“next，等待安装”->“finish”。完成安装之后会弹出补丁安装界面，选择“OK“后，有提示部分补丁读取失败，直接点击确定忽略即可，如图所示：

![](img/chapter02/3.1.1.png) 

在Pack Intaller窗口，依次选择打开“File”->“Impot..”，导入arm mdk 目录下的补丁文件(Keil.STM32F4xx_DFP.1.0.6)，如图所示 ：

![](img/chapter02/3.1.2.png) 

2、运行 MDK(如果是win8 win7  建议用管理员权限打开)，打开”File“下的”License Management...“，复制 CID 编号（11 字符）。 

![](img/chapter02/3.1.3.png) 

3、 解压keygen.zip运行破解软件 keygen.exe。有些杀毒软件（如卡巴斯基）会误报为病毒程序，请临时屏蔽掉杀毒软件后再运行。 

![](img/chapter02/3.1.4.png) 

4、 keygen.exe的“target”下拉列表框选择“ARM”。  第 2 行下拉列表框选择“Prof.Developers Kit/RealView MDK”

5、 将 MDK  的 CID  编号粘贴到 KEIL_lic 中的 CID 编辑框内，然后点击“Generate”按钮。 

![](img/chapter02/3.1.5.png) 

6、 把方框中生成的 30 位注册码，拷贝到 MDK 中的“New License ID Code”内。 

7、 点击“AddLIC”完成注册。如果截止日期太短的话，就继续用 keil_Lic  生成注册码重新注册。 

8、 点击“AddLIC”完成注册。 

### 使用ARM MDK进行ELink407软件开发 ###

#### 第一个工程(工程模版目录结构) ####

良好的目录结构是个好的习惯，这是我们基于固件库第一个工程，以后我们基于固件库开发都以此为工程模版组织代码。
我们建立如下目录
  
.    
├── STM32_Lib   
│   └── __stm32标准库目录   
├── 基础实验   
│   ├── bsp4ELink                             //编写好的驱动源代码   
│   │   │   ├── inc   
│   │   │   ├── __编写的头文件.h   
│   │   └── src   
│   │       ├── __编写的头文件.c   
│   ├── common   
│   │   ├── IAR  
│   │   ├── MDK  
│   │   └── __一些公共的sys文件   
│   └── project                               
│       ├── IAR                             //IAR案例目录   
│       │   └── firstPrj                    //第一个工程，IAR工程模版目录   
│       │       ├── bin                     //生成的烧录二进制文件目录  
│       │       ├── project.ewp            //第一个工程文件   
│       │       ├── list                    //编译过程中生成的lst文件目录   
│       │       ├── obj                     //编译过程中生成的obj文件目录   
│       │       └── user                    //测试驱动代码的main.c等相关文件   
│       │           └── main.c   
│       └── MDK                             //MDK案例目录  
│           └── firstPrj                    //第一个工程，MDK工程模版目录   
│              ├── bin                      //生成的烧录二进制文件目录  
│              ├── project.uvproj             //第一个工程文件   
│              ├── list                     //编译过程中生成的lst文件目录   
│              ├── obj                      //编译过程中生成的obj文件目录   
│              └── user                     //测试驱动代码的main.c等相关文件   
│                  └── main.c   
└── 扩展实验   
    ........    

创建新的工程firstPrj，双击“Keil uVision5”在菜单栏“Project”中选择“New uVision Project”。在firstPrj目录中保存工程文件为firePrj.uvproj，如图：

![](img/chapter02/1.3.4.png) 

保存后，会提示选择CPU型号，在这里我们选择STM32F407VG，如图，弹出窗口再选择OK就可以了。

![](img/chapter06/1.3.3.png) 

### 配置工程  ###
进入工程配置界面的方法： 菜单 --> project -->  Options For Target  或者选择如下图按钮

![](img/chapter06/1.3.5.png) 

1）“Device”选项。选择正确的CPU型号。 

![](img/chapter06/1.3.3.png) 

2）“Target”选项。 

![](img/chapter06/1.3.6.png) 

勾选”Use MicroLIB”。如果不选，会提示__main 函数不存在，启动代码需要跳到__main 然后再跳到用户的main函数中，其次我们习惯调用的printf函数将无法输出信息到串口。 

3）“Output”选项。 

![](img/chapter06/1.3.7.png) 

此处可修改可执行文件名。 勾选“Create HEX File”,表示生成可单独烧写的Hex文件。 取消  “Browse Information”选项可以大大加快每个文件的编译速度。但将丧失鼠标右键定位函数和宏的功能。这个选项不影响最终的Hex。

在编译过程中，会生成大量的obj临时文件，你可以通过“Select Folder for Objects”按钮指定Obj文件的存放目录。这样将是工程文件目录变得非常简洁。 

![](img/chapter06/1.3.8.png) 

4）“Listing”选项。 

![](img/chapter06/1.3.9.png) 

在编译过程中，会生成大量的lst临时文件，你可以通过“Select Folder for Listings”按钮指定lst文件的存放目录。这样将是工程文件目录变得非常简洁。

5）“User”选项。可以指定编译之前和编译之后执行的用户程序。 

为了将编译生成的Hex文件复制到指定目录并更名保存，我们可以在编译后执行一个bat批处理文件。 CopyHex_Flash.bat  文件是一个文本文件，内容如下： copy Flash\Obj\output.hex ..\output(mdk).hex 

6）“C/C++”选项。 

此处“USE_STDPERIPH_DRIVER”表示使用ST的固件库； 

如果有其他的预编译宏，可以用逗号分割。 

Include Path  指定头文件的路搜索路径 添加  --diag_suppress=870 控制，可以避免中文字符串告警 

7）“Asm”选项。无需更改。 

8）“Linker”选项。无需更改。 

9）“Debug”选项。 

点击””按钮设置硬件仿真器，请选择对应的仿真器型号。我们的例程缺省都配置的是ST-Link  仿真器。 勾选“Run to main()”可以在启动Debug时，执行到main()函数暂停。 

我们以ST-Link仿真器为例，点击“Setting”后，界面如下： 

![](img/chapter06/1.3.10.png) 

Port框，请选择”SW”表示使用SWD调试接口。

10）“Utilities”选项。 

点击”Settings”按钮设置硬件仿真器，请选择对应的仿真器型号。我们的例程缺省都配置的是ST-Link  仿真器。 
我们以ST-Link仿真器为例，点击“Setting”后，界面如下： 

![](img/chapter06/1.3.11.png) 

选择“Flash DownLoad”

![](img/chapter06/1.3.12.png) 

点击”Add”按钮，添加Flash编程算法。 

![](img/chapter06/1.3.13.png) 


Project Targets 一栏，我们将 Target 名字修改为 firstPrj,然后在 Groups 一栏删掉一个 Source Group1，建立三个 Groups：sys,drv,bsp,user。然后点击 OK，可以看到我们的 Target名字以及 Groups 情况如下图: 

![](img/chapter02/1.3.10.png) 


我们的 CORE 下面需要添加的文件为 startup_stm32f40_41xxx.s(注意，默认添加的时候文件类型为.c,也就是添加 startup_stm32f40_41xxx.s 启动文件的时候，你需要选择文件类型为 All files才能看得到这个文件 ) ， USER 目录下面需要添加的文件为 main.c ， stm32f4xx_it.c ，system_stm32f4xx.c。  

![](img/chapter06/1.3.11.png) 
                    \source\STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\arm\startup_stm32f40xxx.s
                        \source\STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates\system_stm32f4xx.c

![](img/chapter06/1.3.12.png) 

添加标准设备驱动库文件，源文件所在目录为：

\source_dir\STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\STM32F4xx_StdPeriph_Driver\src
ctrl+a  选择全部，添加后如下图：

![](img/chapter06/1.3.13.png) 

bsp组是我们编写的控制ELink407开发板的具体设备驱动文件，在我们的新工程中留空，以后将目录\基础实验\bsp4ELink\src的文件添加到该组，添加user文件main.c，源文件所在目录为：\基础实验\project\firstPrj\MDK\user

![](img/chapter06/1.3.14.png) 

添加一个全局宏定义标识符。添加方法是点击魔术棒之后，进入 C/C++选项卡，然后在 Define 输入框连输入：STM32F40xxx,USE_STDPERIPH_DRIVER

![](img/chapter06/1.3.16.png) 

添加头文件，设置头文件存放路径。也就是告诉 MDK 到那些目录下面去寻找包含了的头文件。这一步骤非常重要。如果没有设置头文件路径，那么工程会出现报错头文件路径找不到，添加相应的头文件路径:

        基础实验\project\firstPrj\MDK\user
        基础实验\bsp4ELink\inc
        STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\STM32F4xx_StdPeriph_Driver\inc
        STM32_Lib\STM32F4xx_DSP_StdPeriph_Lib\Libraries\CMSIS\Device\ST\STM32F4xx\Include
        
添加后如下图

![](img/chapter06/1.3.17.png) 


11）  配置完毕。

### 下载与调试 ###

新建源文件

![](img/chapter06/1.3.14.png) 

保存文件为main.c

![](img/chapter06/1.3.8.png) 

##IAR for ARM 工具软件的安装使用 ##

IAR Embedded Workbench for ARM version  是一个针对ARM  处理器的集成开发环境，包含项目管理器、编辑器、编译连接工具和支持RTOS  的调试工具，在该环境下可以使用C/C++和汇编语言方便地开发嵌入式应用程序。对于30余种不同架构的处理器，IAR Embedded Workbench提供统一的使用界面，是一个真正的集成开发环境；方便了从8位/16位处理器转向ARM的用户。
### IAR for ARM 安装 ###

在ELink407\tools\EWARM目录下找到安装软件。 

    CD-EWARM-6301-3142.zip  ：安装文件 
    EWARM-6310-3142-CRACK.zip  ：软件注册机 
    
安装步骤： 

1、 将  CD-EWARM-6301-3142.zip  文件解压到硬盘任意文件夹。 

2、 将  EWARM-6310-3142-CRACK.zip  文件解压到硬盘任意文件夹。 

3、 首先运行CRACK下面的  !RUNME.BAT，  会生成license.txt文件  （每台计算机这个文件都不同） 

  a)  用记事本打开license.txt文件，找到第4个Installserial；  
  
  b)  安装EWARM 6.30中，需要系列号时拷贝license.txt文件的第4个Installserial后面的数字；  
  
  c)  安装EWARM 6.30中，需要key时拷贝license.txt文件的与第4个Installserial相接的key后面的长字符串。 
  
4、 运行安装目录下的  autorun.exe  

5、 点击第2项“Install IAR Embedded Workbench”开始安装。等待若干秒后，将欢迎界面： 

6、 点击“Next”按钮继续安装。出现软件授权声明界面： 

7、 选择“I accept the terms of the license agreement  ”同意，然后点击“Next”按钮继续安装。出现输入用户信息的界面： 

8、 在  License#   栏输入第3步中获得  Installserial  数字。  然后点击  Next，出现注册窗口： 

9、 在  License Key   栏输入第3步中获得  Key  数字。  然后点击  Next 

10、  这一步可以选择目标文件夹，我们保持缺省路径不变，点击Next 

11、  选择开始菜单中的程序文件夹名字，一般保持不变，点击Next，出现确认开始安装的界面： 

12、  点击  Install，  开始安装 

13、 等待十几分钟后，出现安装完成的界面 

14、 点击“Finish”按钮。安装成功结束。

### 使用IAR for ARM进行ELink407软件开发

### 第一个工程 ###

见MDK章节

### 工程目录结构 ###

见MDK章节

### 配置工程 ###

进入工程配置界面的方法： 菜单  project  Options... 

1）“General Options”-  “Target”选项。 

“Processor variant”栏选择

“Device”,CPU型号选择  “ST STM32F4xxx” 

“FPU”浮点单元选择“VFPv4” 

2）“General Options - Output”选项。 

可以修改输出文件的目录名称。由于是定位在Flash的工程，我们可以修改名字为Flash。如果是定位在RAM的工程，可以修改名字为CpuRAM。目录名字可以自己随意起。 

3）“General Options - Library Configuration”选项。 

“Library”选择“Full”以支持中文字符串。 

不要勾选“Use CMSIS”，因为我们在源代码中已经明确包含了DSP库的头文件。无需编译器自动寻找。 

4）“General Options - Library Options”选项。保持不变。

随后的“MISRA-C:2004”和“MISRA-C:1998”是C语言语法控制选项，可以保持不变。 

5）“C/C++ Complier - Optimizations”编译优化选项。 

为了调试跟踪方便，我们将C编译器优化级别设置为“None”。如果你的工程对速度和代码有非常严格的要求，可以提高优化级别。 

6）“C/C++ Complier - Preprocessor”C编译器预处理选项。 

在Include  路径中必须正确添加所有的h文件的搜索路径。手工方式。 其中“$PROJ_DIR$”关键字表示工程文件所在的目录。”..\”表示上级目录，连续两个表示上上级目录。 在Define符号栏，必须添加“USE_STDPERIPH_DRIVER”表示使用ST的标准外设固件库。 对于用到USB和以太网的工程，可能还需要添加：USB和PHY相关的宏 

7）“C/C++ Complier”中其它未提及的选项，比如“Output”、“List”等可以保持不变。 

8）“Assembler”汇编器中所有的选项均保持不变。 

9）“Output Converter - Output”选项。 

为了能够编译生成可以直接烧写的可执行程序，需要勾选“Generate additional output”。 输出文件格式一般选择  Intel extened 格式，也就是hex文件。 

输出的缺省hex文件名为 “Project.hex”,存放路径为：  “Fash\Exe\Project.hex”

10）“Build Actions”选项。 

为了将编译生成的Hex文件复制到指定目录并更名保存，我们可以在编译后执行一个bat批处理文件。 CopyHex_Flash.bat  文件是一个文本文件，内容如下： 
        copy Flash\Exe\project.hex ..\output(iar).hex 
11）“Linker - Config”选项。这是比较重要的选项，决定程序定位地址，堆和栈的大小。

“icf”文件用于保存这些参数，我们将其存放工程文件目录下，名字可以自定义。 

点击  “Edit”按钮，弹出对话框，用于设置ROM , RAM, STACK, HEAP。 

12）“Linker - Output”选项。 

在此处可更改输出的目标文件为更有意义的名字。 

13）“Linker - List”选项。 

为了方便查看编译后的ROM、RAM占用大小、函数和变量定位地址、函数大小等信息，请勾选“Generate linker map file”。 
勾选后，连接器将生成一个文本格式的  map文件，可以使用记事本查看，也可以直接双击“Workspace”- “Output”分支下的  map文件打开。 

14）“Linker”中其他未提及的选项，可以保持缺省设置不变。                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        

15）“Debugger - Setup”选项。 

选择使用的仿真器。这里以ST-Link仿真器为例。 

勾选  Run to main，表示启动调试器后，程序执行到  main函数暂停。 

16）“Debugger - Setup”选项。 

请选择  “Use flash loader”，调试器将选择缺省的下载配置文件。 

17）“Debugger”中其它未提及的选项，可以保持缺省参数不变。 

18）“ST-Link/ST-Trace - Setup”保持缺省参数不变。 

19）“ST-Link/ST-Trace - Connection”选项。 

20）其他未提及的选项，均保持缺省值不变。工程配置全部完成。

### 下载与调试 ###

