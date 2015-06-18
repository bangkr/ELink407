# 启动过程 #

STM32F4XX 的启动过程，这里的启动过程是指从 CPU 上电复位执行第 1 条指令开始（汇编文件）到进入 C 程序 main()函数入口之间的部分。启动过程相对来说还是比较重要的，理解了这个过程，对于以后分析程序还是有些帮助的，要不每次看到这个启动过程都会跳过，直接去看主程序了。还有就是以后打算学习 RTOS 的话，对于这个过程必须有个了解，因为移植的时候需要用到这个。

对初学者来说，看这个可能有些吃力，不过不要紧，随着自己做过一些简单的应用之后再来看这章，应该会有很多的帮助，由于我们的ELink407是基于 STM32F407VGT6，所以我们这里主要针对 F4 系列的启动过程做一下分析。 

## 各版本启动文件 ##

这里各个版本的意思是指不同的编译器，不同的 F4 系列对应的启动文件。在这里我们主要分析STM32F407 在MDK和IAR中启动文件 

### MDK启动文件 ###

打开我们为本期教程提供的工程文件，路径如下： \Libraries\CMSIS\Device\ST\STM32F4xx\Source\Templates  在这个文件里面有 ST 官方为各个编译器提供的启动文件。 

MDK 带的启动文件    
STM32F40xx  对应的如下系列   
STM32F405RG,  STM32F405VG,  STM32F405ZG,  STM32F415RG,  STM32F415VG, STM32F415ZG,STM32F407VG,  STM32F407VE,  STM32F407ZG,  STM32F407ZE, STM32F407IG, STM32F407IE,  STM32F417VG,  STM32F417VE,  STM32F417ZG,  STM32F417ZE,  STM32F417IG, STM32F417IE。 

### IAR 启动文件 ###

我们再来打开 IAR 文件夹里面的文件，对比下 MDK 和 IAR 带的文件，我们可以发现，除了文件大小不一样以外，文件名字是一样的。这个就跟大家说这么多，有兴趣的可以看看为其它编译器提供的启动文件。

## 启动代码分析 ##

下面我们详细的分析一下启动文件startup_stm32f40_41xxx.s。说之前还得需要大家看一下 F4 系列的内存映射，记住 SRAM 的首地址和 FLASH 的首地址。 

阅读汇编程序必须对汇编语言中的关键字有一定了解。如果你不能从字面意思理解汇编语句，那么可以使用 MDK 的联机帮助来查阅。具体方法如下： 

启动  MDK 软件，在 Help 菜单点击  uVision Help 执行后将出现帮助文件窗口 

在搜索栏输入你需要查询的单词进行查询即可。   

我们先来看一下，启动文件前面的介绍  （固件库版本：V1.5.0） 

        ;**********(C) COPYRIGHT 2012 STMicroelectronics ***********
        ;* File Name : startup_stm32f4xx.s   
        ;* Author : MCD Application Team   
        ;* Version : V1.5.0   
        ;* Date : 05-March-2015   
        ;* Description : STM32F4xx devices vector table for MDK-ARM toolchain.  
        ;* This module performs:  
        ;* - Set the initial SP  
        ;* - Set the initial PC == Reset_Handler  
        ;* - Set the vector table entries with theexceptions ISR address   
        ;* - Configure the system clock and the external SRAM mounted on   
        ;* STM324xG-EVAL board to be used as data memory (optional,   
        ;* to be enabled by user)   
        ;* - Branches to __main in the C library (which eventually   
        ;* calls main()).   
        ;* After Reset the CortexM4 processor is in Thread mode,   
        ;* priority is Privileged, and the Stack is set to Main.   
        ;* <<< Use Configuration Wizard in Context Menu >>>   
        ;*********************************************************   
        ;   
        ; Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");   
        ; You may not use this file except in compliance with the License.   
        ; You may obtain a copy of the License at:    
        ;   
        ; http://www.st.com/software_license_agreement_liberty_v2   
        ;   
        ; Unless required by applicable law or agreed to in writing, software   
        ; distributed under the License is distributed on an "AS IS" BASIS,  
        ; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   
        ; See the License for the specific language governing permissions and   
        ; limitations under the License.   
        ;   
        ;***********************************************************   
        ; Amount of memory (in bytes) allocated for Stack    
        ; Tailor this value to your application needs   
        ; <h> Stack Configuration   
        ; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>   
        ; </h>   
        
启动文件是后缀为.s 的汇编语言文本文件。行首的分号表示此行是注释行。 
启动文件主要完成如下工作，即程序执行过程 

*  -  设置堆栈指针 SP = __initial_sp   
*  -  设置 PC 指针  = Reset_Handler   
*  -  配置系统时钟    
*  -  配置外部 SRAM 用于程序变量等数据存储（这是可选的）   
*  -  跳转（Jump）到 C 库中的  __main  ，最终会调用（Call）用户程序的 main()函数   

CortexM4 处理器复位后，处于线程模式，指令权限是特权级别（最高级别），堆栈设置为主堆栈。  

硬件复位之后，CPU 内的时序逻辑电路首先将 0x0800  0000 位置存放的堆栈栈顶地址装入 SP 寄存器。紧接着将 0x0800  0004 位置存放的向量地址装入 PC 程序计数器。CPU 从 PC 寄存器指向的物理地址取出第 1 条指令开始执行程序，也就是开始执行复位中断服务程序Reset_Handler.  

复位中断服务程序会调用 SystemInit()函数(C 语言的)  来配置系统时钟、配置 FSMC 总线上的外部SRAM，然后跳转到 C 库中__main 函数。由 C 库中的__main 函数完成用户程序的初始化工作（比如：变量赋初值等），最后由__main 函数调用用户写的  main()函数开始执行 C 程序。   

        Stack_Size EQU 0x00004000 
        AREA STACK, NOINIT, READWRITE, ALIGN=3 
        Stack_Mem SPACE Stack_Size 
        __initial_sp 
        
MDK 汇编语言的语法和关键字定义可以在 MDK 的连接帮助中找到定义。   

EQU 是表示宏定义的伪指令，类似于 C 语言中的#define。伪指令的意思是指这个“指令”并不会生成二进制程序代码，也不会引起变量空间分配。 0x00004000 表示堆栈大小，注意这里是以字节为单位。 开辟一段数据空间可读可写，段名 STACK，按照 8 字节对齐。   

ARER 伪指令表示下面将开始定义一个代码段或者数据段。此处是定义数据段。ARER 后面的关键字表示这个段的属性。    

 * STACK  ：表示这个段的名字，可以任意命名。    
 * NOINIT：表示此数据段不需要填入初始数据。   
 * READWRITE：表示此段可读可写。    
 * ALIGN=3  ：表示首地址按照 2 的 3 次方对齐，也就是按照 8 字节对齐。   

SPACE  这行指令告诉汇编器给 STACK 段分配 0x00004000 字节的连续内存空间。   

__initial_sp 只是一个标号。这里解释一下什么是标号，标号主要用于表示一片内存空间的某个位置，等价于 C 语言中的“地址”概念。地址仅仅表示存储空间的一个位置，从 C 语言的角度来看，变量的地址，数组的地址或是函数的入口地址在本质上并无区别。 

__initial_sp 紧接着 SPACE 语句放置，表示了栈空间顶地址。M4 堆栈是由高地址空间向低地址空间增长的。压栈（PUSH）时，堆栈指针 SP 递减。栈（POP）时，SP 递增。 栈（STACK）用于存储局部变量、保存函数返回地址。 

        ; <h> Heap Configuration 
        ; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8> 
        ; </h> 
        Heap_Size EQU 0x00000400 
        AREA HEAP, NOINIT, READWRITE, ALIGN=3 
        __heap_base 
        Heap_Mem SPACE Heap_Size 
        __heap_limit 
        
这几行语句和上面的一段代码类似。分配一片连续的内存空间给名字叫 HEAP 的段，也就是分配堆空间。堆的大小为 0x00000400。堆的首地址是 8 字节对齐。堆主要用于动态内存分配，也就是说用 malloc 函数分配的空间位于堆空间。  

__heap_base  表示堆的开始地址。 

__heap_limit  表示堆的结束地址。 

PRESERVE8 

THUMB 

PRESERVE8 指定当前文件保持堆栈八字节对齐。 

THUMB 表示后面的指令是 THUMB 指令集  （CM4 采用的是 16 位 THUMB 指令集，这是相对于ARM7，ARM9，ARM11 的 32 位的 ARM 指令集而言的） 

        ; Vector Table Mapped to Address 0 at Reset 
        AREA RESET, DATA, READONLY 
        EXPORT __Vectors 
        EXPORT __Vectors_End 
        EXPORT __Vectors_Size 
        
AREA 定义一块代码段，只读，段名字是 RESET。READONLY 表示只读，缺省就表示代码段了。 3 行 EXPORT 语句将 3 个标号申明为可被外部引用，主要提供给连接器用于连接库文件或其他其他文件。 

        __Vectors DCD __initial_sp ; Top of Stack 
        DCD Reset_Handler ; Reset Handler 
        DCD NMI_Handler ; NMI Handler 
        DCD HardFault_Handler ; Hard FaultHandler 
        DCD MemManage_Handler ; MPU Fault Handler 
        DCD BusFault_Handler ; Bus Fault Handler 
        DCD UsageFault_Handler ; Usage Fault Handler 
        … 中间的代码已省略 … 
        DCD DCMI_IRQHandler ; DCMI 
        DCD CRYP_IRQHandler ; CRYP crypto 
        DCD HASH_RNG_IRQHandler ; Hash and Rng 
        DCD FPU_IRQHandler ; FPU
        __Vectors_End 
        __Vectors_Size EQU __Vectors_End - __Vectors 
        
上面的这段代码是建立中断向量表，中断向量表定位在代码段的最前面。具体的物理地址由连接器的配置参数（IROM1 的地址）决定。如果程序在 Flash 运行，则中断向量表的起始地址是 0x08000000。 DCD 表示分配 1 个 4 字节的空间。每行 DCD 都会生成一个 4 字节的二进制代码。中断向量表存放的实际上是中断服务程序的入口地址。当异常（也即是中断事件）发生时，CPU 的中断系统会将相应的入口地址赋值给 PC 程序计数器，之后就开始执行中断服务程序。 

        AREA |.text|, CODE, READONLY 
        ; Reset handler 
        Reset_Handler PROC 
        EXPORT Reset_Handler [WEAK] 
        IMPORT SystemInit 
        IMPORT __main 
        LDR R0, =SystemInit 
        BLX R0 
        LDR R0, =__main 
        BX R0 
        ENDP 
        … 下面是其他的中断服务程序 … 
        ; Dummy Exception Handlers (infinite loops which can be modified) 
        NMI_Handler PROC 
        EXPORT NMI_Handler [WEAK] 
        B . <------ 死循环，用户可以自己编写中断服务程
        ENDP 
        HardFault_Handler\ 
        PROC 
        EXPORT HardFault_Handler [WEAK] 
        B . 
        ENDP 
        … 中间的代码已省略 … 
        Default_Handler PROC  <------ 缺省的中断服务程序（开始）
        EXPORT WWDG_IRQHandler [WEAK] 
        EXPORT PVD_IRQHandler [WEAK] 
        … 中间的代码已省略 … 
        CRYP_IRQHandler 
        HASH_RNG_IRQHandler 
        FPU_IRQHandler 
        B . <------ 死循环
        ENDP  <------ 缺省的中断服务程序(结束) 
        
上面的程序就是用汇编代码实现的中断服务程序，下面还有其它中断的中断服务程序，这里就不说了，重点把这个复位中断服务程序说一下。    

利用 PROC、ENDP 这一对伪指令把程序段分为若干个过程，使程序的结构加清晰。    

 WEAK 声明其他的同名标号优先于该标号被引用,就是说如果外面声明了的话会调用外面的。这个申明很重要，它让我们可以在 C 文件中任意地方放置中断服务程序，只要保证 C 函数的名字和向量表中的名字一致即可。   
 
IMPORT：伪指令用于通知编译器要使用的标号在其他的源文件中定义。但要在当前源文件中引用，而且无论当前源文件是否引用该标号，该标号均会被加入到当前源文件的符号表中。   

SystemInit 函数在文件 system_stm32f4xx.c 里面，这个文件在时钟系统有详细讲解。   

这里重点说明一下__main 标号，__main 标号并不表示 C 程序中的 main 函数入口地址， 因此 LDR R0，=_main 也并不是跳转至 main 函数开始执行 C 程序。__main 标号表示 C/C++标准实时库函数里的一个初始化子程序__main 的入口地址。该程序的一个主要作用是初始化堆栈（跳转__user_initial_stackheap 标号进行初始化堆栈的，下面会讲到这个标号），并初始化映像文件，最后跳转到 C 程序中的 main 函数。这就解释了为何所有的 C 程序必须有一个 main 函数作为程序的起点。因为这是由 C/C++标准实时库所规，并且不能更改。 下面我们来看看启动文件最后一段代码，选择用户自己初始化堆和栈。 

        ALIGN 
        ;*********************************** 
        ; User Stack and Heap initialization 
        ;*********************************** 
        IF :DEF:__MICROLIB 
        EXPORT __initial_sp 
        EXPORT __heap_base 
        EXPORT __heap_limit 
        ELSE 
        IMPORT __use_two_region_memory 
        EXPORT __user_initial_stackheap 
        __user_initial_stackheap 
        LDR R0, = Heap_Mem 
        LDR R1, =(Stack_Mem + Stack_Size) 
        LDR R2, = (Heap_Mem + Heap_Size) 
        LDR R3, = Stack_Mem 
        BX LR 
        ALIGN 
        ENDIF 
        END 
        ;********* (C) COPYRIGHT STMicroelectronics *****END OF FILE*****
        
上面就是一个简单的汇编语言实现 IF…….ELSE…………语句。如果定义了 MICROLIB，那么程序是不会 ELSE 分支的代码。 __MICROLIB 可能大家并不陌生，就在 MDK 的 Target Option 里面设置。

