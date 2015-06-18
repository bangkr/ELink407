# LCD 实验 #

编写LCD驱动，通过 STM32F4 的 FSMC 接口来控制 LCD 进行显示。 

## 实验原理与基础知识 ##

LCD 液晶屏是 Liquid Crystal Display 的简称，屏幕能显示的基本原理就是在两片平行的玻璃当中放置液态的晶体，两片玻璃中间有许多垂直和水平的细小电线，通过通电与否或电压的大小改变来控制液晶分子的转动方，以达到遮光和透光的目的来显示深浅不一，错落有致的图象，而且只要在两块平板间再加上三元色的滤光层，就可实现显示彩色图象。LCD显示原理如图所示

![](img/chapter24/1.1.1.png) 

因此要在LCD上显示一幅画的过程就是在正确的位置写入正确的电压，如果要维持该画面的显示就需要保持该位置的电压，通常情况下LCD控制电路会完成定时刷新电压的工作，这个跟RAM控制器非常类似。但是根据LCD不一样，具体配置相应的控制寄存器。Elink407使用的是LCD模组是LX T30WH006T1，具体参见LCD的datasheet。

LCD与stm32F4通过FSMC（灵活的静态存储控制器）接口连接，FSMC，即灵活的静态存储控制器，能够与同步或异步存储器和 16 位 PC 存储器卡连接，STM32F4 的 FSMC 接口支持包括 SRAM、 NAND FLASH、 NOR FLASH 和 PSRAM 等存储器。FSMC 的框图。

![](img/chapter24/1.1.2.png) 

FSMC 包含四个主要模块：

 * ● AHB 接口（包括FSMC 配置寄存器）
 
 * ● NOR Flash/PSRAM 控制器
 
 * ● NAND Flash/PC 卡控制器
 
 * ● 外部器件接口
 
STM32F4 的 FSMC 将外部设备分为 2 类：NOR/PSRAM 设备、NAND/PC 卡设备。他们共用地址数据总线等信号，他们具有不同的 CS 以区分不同的设备,将 LCD 当成 RAM 来控制,LCD的地址线、数据线、写信号（WE）、读信号（OE）、片选信号（CS），命令数据选择信号(RS)，分别与FSMC对应的功能管脚相连，需要说明的是 LCD 有 RS 信号，但是没有地址信号,我们把 RS 接在 A16 上面，那么当 FSMC 控制器写地址 0 的时候，就是写命令。写地址 1 的时候，就是写数据了。这样实现在同一数据总线上面区分地址和数据。

STM32F4 的 FSMC 支持 8/16/32 位数据宽度，我们这里用到的 LCD 是 16 位宽度的，所以在设置的时候，选择 16 位宽。STM32F4的 FSMC 将外部存储器划分为固定大小为 256M 字节的四个存储块，如图所示FSMC 存储块地址映像

![](img/chapter24/1.1.3.png) 

FSMC 总共管理 1GB 空间，拥有 4 个存储块（Bank），LCD用到的是NOR/PSRAM块, 该区块可连接多达4 个NOR Flash 或PSRAM 存储器器件。此存储区域被划分为4 个 NOR/PSRAM 区域，带4 个专用片选信号,内 部 AHB 地址总 线HADDR[25:0]用来表示外部存储器地址,而HADDR[27:26] 位用于四个存储区域之中选择其中一个存储区域如表所示。

![](img/chapter24/1.1.4.png) 

对于 NOR/PSRAM控制器，主要是通过 FSMC_BCRx、FSMC_BTRx 和 FSMC_BWTRx 寄存器设置（其中 x=1~4，对应 4 个区,Elink407 LCD使用的是1）。通过这 3 个寄存器，可以设置 FSMC 访问外部存储器的时序参数。

FSMC 的 NOR FLASH 控制器支持同步和异步突发两种访问方式。选用同步突发访问方式时，FSMC 将 HCLK(系统时钟)分频后，发送给外部存储器作为同步时钟信号 FSMC_CLK。此时需要的设置的时间参数有 2 个：

1，HCLK 与 FSMC_CLK 的分频系数(CLKDIV)，可以为 2～16 分频；

2，同步突发访问中获得第 1 个数据所需要的等待延迟(DATLAT)。

对于异步突发访问方式，FSMC 主要设置 3 个时间参数：地址建立时间(ADDSET)、数据建立时间(DATAST)和地址保持时间(ADDHLD)。 

![](img/chapter24/1.1.5.png) 

FSMC 定义了 4 种不同的异步时序模型(A,B,C,D)。选用不同的时序模型时，需要设置不同的时序参数，ELink407 LCD 使用的是A模式，其读写时序如图所示

![](img/chapter24/1.1.6.png) 

![](img/chapter24/1.1.7.png) 

FSMC_BCRx 位字段

![](img/chapter24/1.1.8.png) 

FSMC_BTRx 位字段

![](img/chapter24/1.1.9.png) 

FSMC_BWTRx 位字段

![](img/chapter24/1.1.10.png) 

NOR/PSRAM 控制寄存器

**SRAM/NOR-Flash 片选控制寄存器**
偏移地址：0xA000 0000 + 8 * (x — 1)，x = 1...4
    
![](img/chapter24/1.1.11.png) 

**位31：20**  保留，必须保持复位值。

**位19**     CBURSTRW：写入突发使能(Write burst enable)。对于Cellular RAM (PSRAM)，该位可在写操作时使能同步突发协议。读取访问期间同步突发协议的使能位为FSMC_BCRx 寄存器中的BURSTEN 位。

 * 0：始终在异步模式下进行写入操作
 
 * 1：在同步模式下进行写入操作。

**位18：16**  保留，必须保持复位值。

**位15**     ASYNCWAIT：异步传输期间的等待信号(Wait signal during asynchronous transfers)该位可使能/禁止FSMC 使用等待信号，即使是在异步协议期间该位也有作用。

 * 0：运行异步协议时不考虑NWAIT 信号（复位后的默认值）
 
 * 1：运行异步协议时考虑NWAIT 信号

**位14**     EXTMOD：扩展模式使能(Extended mode enable)。FSMC 可对FSMC_BWTR 寄存器中的写入时间进行配置，此配置由EXTMOD 位使能，进而使读取和写入操作采用不同时序。

 * 0：不考虑FSMC_BWTR 寄存器中的值（复位后的默认值）
 * 1：考虑FSMC_BWTR 寄存器中的值

**位13**     WAITEN：等待使能位(Wait enable bit)。该位可使能/禁止在同步模式下访问Flash 时通过NWAIT 信号插入等待周期。

 * 0：禁止NWAIT 信号（不考虑其电平，不在配置过的Flash 延迟周期后插入等待周期）
 
 * 1：使能NWAIT 信号（考虑其电平，如果使能，在配置过的Flash 延迟周期后插入等待周期）（复位后的默认值）

**位12**     WREN：写入使能位(Write enable bit)。该位指示FSMC 是否在存储区域内使能/禁止了写入操作：

 * 0：FSMC 在存储区域内禁止了写入操作，如果进行写操作将报告AHB 错误，
 
 * 1：FSMC 在存储区域内使能了写入操作（复位后的默认值）。

**位11**     WAITCFG：等待时序配置(Wait timing configuration)。NWAIT 信号指示存储器中的数据是否有效，或者在同步模式下访问Flash 时是否必须插入等待周期。该配置位决定存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT：

 * 0：NWAIT 信号在等待周期之前的一个数据周期有效（复位后的默认值），
 
 * 1：NWAIT 信号在等待周期期间有效（不适用于Cellular RAM）。

**位10**     WRAPMOD：环回突发模式支持(Wrapped burst mode support)。定义控制器是否会将一个AHB 突发环回访问分割成为两个线性访问。仅在突发模式下访 问存储器时有效
 * 0：未使能直接环回突发（复位后的默认值），
 * 1：使能直接环回突发。

**位9**      WAITPOL：等待信号极性位(Wait signal polarity bit)。定义存储器的等待信号极性。仅在突发模式下访问存储器时有效：

 * 0：NWAIT 低电平有效（复位后的默认值），
 
 * 1：NWAIT 高电平有效。

**位8**     BURSTEN：突发使能位(Burst enable bit)。该位可使能/禁止读取操作期间的同步突发访问。仅对同步突发存储器有效：

 * 0：禁止突发访问模式（复位后的默认值）
 
 * 1：使能突发访问模式

**位7**      保留，必须保持复位值。

**位6**      FACCEN：Flash 访问使能(Flash access enable)使能NOR Flash 访问操作。

 * 0：禁止相应的NOR Flash 访问
 
 * 1：使能相应的NOR Flash 访问（复位后的默认值）

**位5:4**    MWID：存储器数据总线宽度(Memory databus width)。定义外部存储器器件宽度，对所有类型的存储器均有效。

 * 00：8 位，
 
 * 01：16 位（复位后的默认值），
 
 * 10：保留，不使用，
 
 * 11：保留，不使用。

**位3:2**    MTYP：存储器类型(Memory type)。定义与相应存储区域相连的外部存储器类型：

 * 00：SRAM、ROM（对于存储区域2...4，复位后的默认值）
 
 * 01：PSRAM（Cellular RAM：CRAM）
 
 * 10：NOR Flash/OneNAND Flash （对于存储区域1，复位后的默认值）
 
 * 11：保留

**位1**      MUXEN：地址/数据复用使能位(Address/data multiplexing enable bit)。该位置1 时，地址和数据值在数据总线上复用，仅对NOR 和PSRAM 存储器有效：

 * 0：地址/数据非复用

 * 1：地址/数据在数据总线上复用（复位后的默认值）

**位0**      MBKEN：存储区域使能位(Memory bank enable bit)。使能存储区域。复位后使能存储区域1，其它存储区域均禁止。访问禁止的存储区域会引起AHB 总线上的错误。

 * 0：禁止相应的存储区域
 
 * 1：使能相应的存储区域

**SRAM/NOR-Flash 片选时序寄存器(FSMC_BTRx)**

偏移地址：0xA000 0000 + 0x04 + 8 * (x — 1)，x = 1..4

该寄存器包含每个存储区域的控制信息，用于SRAM、ROM 和NOR Flash。如果FSMC_BCRx 寄存器中的EXTMOD 位置1，该寄存器将和另外一个寄存器配合来配置写入和读取的时序参数。也就是说有2 个寄存器可用：一个用于配置读取访问（此寄存器），另一个用于配置写入访问（FSMC_BWTRx 寄存器）。

![](img/chapter24/1.1.12.png) 

**位31:30**      保留，必须保持复位值。

**位29:28**  ACCMOD：访问模式(Access mode)指定异步访问模式，如时序图所示。仅当FSMC_BCRx 寄存器中的ECTMOD 位为1 时才考虑这些位。

 * 00：访问模式A
 
 * 01：访问模式B
 
 * 10：访问模式C
 
 * 11：访问模式D

**位27:24**  DATLAT：同步突发NOR Flash 的数据延迟(Data latency for synchronous burst NOR Flash memory)对于使能了同步突发模式的NOR Flash，该位定义了获取首个数据前要发送给存储器的存储器 时钟周期数(+2)：

 * 0000：首次突发访问时，2 个CLK 时钟周期的数据延迟
 
 * 1111：首次突发访问时，17 个CLK 时钟周期的数据延迟（复位后的默认值）
 
注意：该时序参数以Flash 时钟(CLK) 周期而非HCLK 周期表示。在异步NOR Flash、SRAM 或ROM 访问模式下，该值为无关值。如果是CRAM，该字段必须设置为“0”。

**位23:20**  CLKDIV：CLK 信号的时钟分频比(Clock divide ratio (for CLK signal))定义CLK 时钟输出信号的周期，以HCLK 周期数表示：

 * 0000：Reserved
 
 * 0001：CLK 周期= 2 × HCLK 周期
 
 * 0010：CLK 周期= 3 × HCLK 周期
 
 * 1111：CLK 周期= 16 × HCLK 周期（复位后的默认值）
 
在异步NOR Flash、SRAM 或ROM 访问模式下，该值为无关值。

**位19:16**  BUSTURN：总线周转阶段的持续时间(Bus turnaround phase duration)通过软件写入这些位可在写入/读取事务的结尾添加延迟。该延迟可以匹配连续事务之间的最短时间（tEHEL 由NEx 高电平变为NEx 低电平）以及存储器在读取访问后释放数据总线所需 的最长时间(tEHQZ)：

(BUSTRUN + 1)HCLK 周期 tEHELmin 和(BUSTRUN + 2)HCLK 周期 tEHQZmax（如果EXTMOD =“0”）

(BUSTRUN + 2)HCLK 周期 max (tEHELmin，tEHQZmax)（如果EXTMOD =“1”）。

 * 0000：BUSTURN 阶段的持续时间= 增加0 个HCLK 时钟周期
 
 * ...
 
 * 1111：BUSTURN 阶段的持续时间= 15 × HCLK 时钟周期（复位后的默认值）

**位15:8**   DATAST：数据阶段的持续时间(Data-phase duration)通过软件写入这些位可定义数据阶段的持续时间，适用于SRAM、ROM 和异步NOR Flash 访问模式：

 * 0000 0000：Reserved
 
 * 0000 0001：DATAST 阶段的持续时间= 1 × HCLK 时钟周期 
 
 * 0000 0010：DATAST 阶段的持续时间= 2 × HCLK 时钟周期 
 
 * ...
 
 * 1111 1111：DATAST 阶段的持续时间= 255 × HCLK 时钟周期（复位后的默认值）

**位7:4**    ADDHLD：地址保持阶段的持续时间(Address-hold phase duration)通过软件写入这些位可定义地址保持阶段的持续时间，适用于模式 D 和复用访问：

 * 0000：Reserved
 
 * 0001：ADDHLD 阶段的持续时间= 1 × HCLK 时钟周期 
 
 * 0010：ADDHLD 阶段的持续时间= 2 × HCLK 时钟周期 
 
 * ...
 
 * 1111：ADDHLD 阶段的持续时间= 15 × HCLK 时钟周期（复位后的默认值）

**位3:0**     ADDSET：地址设置阶段的持续时间(Address setup phase duration)通过软件写入这些位可定义地址设置阶段的持续时间，适用于SRAM、ROM 和异步NOR Flash 访问模式：

 * 0000：ADDSET 阶段的持续时间= 0 × HCLK 时钟周期 
 
 * ...
 
 * 1111：ADDSET 阶段的持续时间= 1615 × HCLK 时钟周期（复位后的默认值）

**SRAM/NOR-Flash 写入时序寄存器 (FSMC_BWTRx)**

偏移地址：0xA000 0000 + 0x104 + 8 * (x — 1)，x = 1...4

该寄存器包含每个存储区域的控制信息，用于SRAM、ROM 和NOR Flash。当FSMC_BCRx 寄存器中的EXTMOD 位置1 时，该寄存器将处于有效状态，可以进行写入访问。

![](img/chapter24/1.1.13.png) 

**位31:30**  保留，必须保持复位值。

**位29:28**  ACCMOD：访问模式(Access mode)。指定异步访问模式，如下一个时序图所示。仅当FSMC_BCRx 寄存器中的EXTMOD 位置1 时 ， 这些位才有效。

 * 00：访问模式A
 
 * 01：访问模式B
 
 * 10：访问模式C
 
 * 11：访问模式D 

**位27:24**  DATLAT：数据延迟（用于同步突发NOR Flash）(Data latency (for synchronous burst NOR Flash))。对于使能了同步突发模式的NOR Flash，该位定义了获取首个数据前要发送给存储器的存储器时钟 周期数(+2)：

 * 0000：(0x0) 首次突发访问时，2 个CLK 时钟周期的数据延迟
 
 * ...
 
 * 1111：(0xF) 首次突发访问时，17 个CLK 时钟周期的数据延迟（复位后的默认值）

**位23:20**  CLKDIV：CLK 信号的时钟分频比(Clock divide ratio (for CLK signal))。定义CLK 时钟输出信号的周期，以HCLK 周期数表示：

 * 0000：保留
 
 * 0001 CLK 周期= 2 × HCLK 周期
 
 * 0010 CLK 周期= 3 × HCLK 周期
 
 * 1111：CLK 周期= 16 × HCLK 周期（复位后的默认值）

**位19:16**  BUSTURN：总线周转阶段的持续时间(Bus turnaround phase duration)通过软件写入这些位可在写入事务结束时增加一个延迟，进而匹配两个连续事务之间的最小时间

（tEHEL 由ENx 高电平变为ENx 低电平）：(BUSTRUN + 1) HCLK 周期 tEHELmin。

 * 0000：BUSTURN 阶段的持续时间= 增加0 个HCLK 时钟周期
 
 * ... 
 
* 1111：BUSTURN 阶段的持续时间= 增加15 个HCLK 时钟周期（复位后的默认值）

**位15:8**   DATAST：数据阶段的持续时间(Data-phase duration)。通过软件写入这些位可定义数据阶段的持续时间，适用于SRAM、ROM 和异步NOR Flash 访问模式：

 * 0000 0000：保留
 
 * 0000 0001:DATAST 阶段的持续时间= 1 × HCLK 时钟周期 
 
 * 0000 0010:DATAST 阶段的持续时间= 2 × HCLK 时钟周期 
 
 * ...
 
 * 1111 1111:DATAST 阶段的持续时间= 255 × HCLK 时钟周期（复位后的默认值）

**位7:4**    ADDHLD：地址保持阶段的持续时间(Address-hold phase duration)。通过软件写入这些位可定义地址保持 阶段的持续时间，适用于SRAM、ROM 和异步复用NOR Flash 访问模式：

 * 0000：保留
 
 * 0001：ADDHLD 阶段的持续时间= 1 × HCLK 时钟周期
 
 * 0010：ADDHLD 阶段的持续时间= 2 × HCLK 时钟周期
 
 * ...
 
 * 1111：ADDHLD 阶段的持续时间= 15 × HCLK 时钟周期（复位后的默认值）

**位3:0**    ADDSET：地址设置阶段的持续时间(Address setup phase duration)。通过软件写入这些位可定义以HCLK 周期表示的地址设置 阶段的持续时间，适用于SRAM、ROM 和异步NOR Flash 访问模式：

 * 0000：ADDSET 阶段的持续时间= 0 × HCLK 时钟周期

 * ...

 * 1111：ADDSET 阶段的持续时间= 15 × HCLK 时钟周期（复位后的默认值）

## 电路设计 ##

![](img/chapter0/lcd_sch.png) 

## 库函数和数据结构 ##
相关的操作在文件 stm32f4xx_fsmc.c 和对应的头文件 stm32f4xx_fsmc.h 中

NORSRAM初始化结构体，前面有 13 个32位整形的成员变量，是用来配置片选控制寄存器 FSMC_BCRx。最后面还有两个SMC_NORSRAMTimingInitTypeDef 指针类型的成员变量是用来设置读时序和写时序的参数。

        /** 
          * @brief  FSMC NOR/SRAM Init structure definition
          */

        typedef struct
        { 
            /*!< Specifies the NOR/SRAM memory bank that will be used.
            This parameter can be a value of @ref FSMC_NORSRAM_Bank */
          uint32_t FSMC_Bank;                

            /*!< Specifies whether the address and data values are
            multiplexed on the data bus or not. 
            This parameter can be a value of @ref FSMC_Data_Address_Bus_Multiplexing */
          uint32_t FSMC_DataAddressMux;      

            /*!< Specifies the type of external memory attached to
            the corresponding memory bank.
            This parameter can be a value of @ref FSMC_Memory_Type */
          uint32_t FSMC_MemoryType;          

            /*!< Specifies the external memory device width.
            This parameter can be a value of @ref FSMC_Data_Width */
          uint32_t FSMC_MemoryDataWidth;     

            /*!< Enables or disables the burst access mode for Flash memory,
            valid only with synchronous burst Flash memories.
            This parameter can be a value of @ref FSMC_Burst_Access_Mode */
          uint32_t FSMC_BurstAccessMode;     

            /*!< Enables or disables wait signal during asynchronous transfers,
            valid only with asynchronous Flash memories.
            This parameter can be a value of @ref FSMC_AsynchronousWait */  
          uint32_t FSMC_AsynchronousWait;     

            /*!< Specifies the wait signal polarity, valid only when accessing
            the Flash memory in burst mode.
            This parameter can be a value of @ref FSMC_Wait_Signal_Polarity */
          uint32_t FSMC_WaitSignalPolarity;  

            /*!< Enables or disables the Wrapped burst access mode for Flash
            memory, valid only when accessing Flash memories in burst mode.
            This parameter can be a value of @ref FSMC_Wrap_Mode */
          uint32_t FSMC_WrapMode;            

            /*!< Specifies if the wait signal is asserted by the memory one
            clock cycle before the wait state or during the wait state,
            valid only when accessing memories in burst mode. 
            This parameter can be a value of @ref FSMC_Wait_Timing */
          uint32_t FSMC_WaitSignalActive;    

            /*!< Enables or disables the write operation in the selected bank by the FSMC. 
            This parameter can be a value of @ref FSMC_Write_Operation */
          uint32_t FSMC_WriteOperation;      

            /*!< Enables or disables the wait state insertion via wait
            signal, valid for Flash memory access in burst mode. 
            This parameter can be a value of @ref FSMC_Wait_Signal */
          uint32_t FSMC_WaitSignal;          

            /*!< Enables or disables the extended mode.
            This parameter can be a value of @ref FSMC_Extended_Mode */
          uint32_t FSMC_ExtendedMode;        

            /*!< Enables or disables the write burst operation.
            This parameter can be a value of @ref FSMC_Write_Burst */ 
          uint32_t FSMC_WriteBurst;          

          /*!< Timing Parameters for write and read access if the  Extended Mode is not used*/  
          FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; 

              /*!< Timing Parameters for write access if the  Extended Mode is used*/      
          FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct; 

          }FSMC_NORSRAMInitTypeDef;
          
FSMC 有读时序和写时序之分，这里定义的FSMC_NORSRAMTimingInitTypeDef结构体，用来设置读时序和写时序的参数，即配置寄存器 FSMC_BTRx 和 FSMC_BWTRx。 结构体有 7 个参数用来设置 FSMC 读写时序，主要是设计地址建立保持时间，数据建立时间等等配置 。

          /** 
            * @brief  Timing parameters For NOR/SRAM Banks  
            */
        typedef struct
        {
            /*!< Defines the number of HCLK cycles to configure
            the duration of the address setup time. 
            This parameter can be a value between 0 and 0xF.
            @note This parameter is not used with synchronous NOR Flash memories. */
            uint32_t FSMC_AddressSetupTime;       

            /*!< Defines the number of HCLK cycles to configure
            the duration of the address hold time.
            This parameter can be a value between 0 and 0xF. 
            @note This parameter is not used with synchronous NOR Flash memories.*/
            uint32_t FSMC_AddressHoldTime;        

            /*!< Defines the number of HCLK cycles to configure
             the duration of the data setup time.
             This parameter can be a value between 0 and 0xFF.
             @note This parameter is used for SRAMs, ROMs and asynchronous 
             multiplexed NOR Flash memories. */
            uint32_t FSMC_DataSetupTime;          

            /*!< Defines the number of HCLK cycles to configure
            the duration of the bus turnaround.
            This parameter can be a value between 0 and 0xF.
            @note This parameter is only used for multiplexed NOR Flash memories. */
            uint32_t FSMC_BusTurnAroundDuration;  

            /*!< Defines the period of CLK clock output signal, expressed in 
            number of HCLK cycles.
            This parameter can be a value between 1 and 0xF.
            @note This parameter is not used for asynchronous NOR Flash, 
            SRAM or ROM accesses. */
            uint32_t FSMC_CLKDivision;            

            /*!< Defines the number of memory clock cycles to issue
            to the memory before getting the first data.
            The parameter value depends on the memory type as shown below:
            - It must be set to 0 in case of a CRAM
            - It is don't care in asynchronous NOR, SRAM or ROM accesses
            - It may assume a value between 0 and 0xF in NOR Flash memories
            with synchronous burst mode enable */
            uint32_t FSMC_DataLatency;            

            /*!< Specifies the asynchronous access mode. 
            This parameter can be a value of @ref FSMC_Access_Mode */
            uint32_t FSMC_AccessMode;             

        }FSMC_NORSRAMTimingInitTypeDef;
  
        void RCC_AHB3PeriphClockCmd(uint32_t RCC_AHB3Periph, FunctionalState NewState)  
FSMC 时钟使能函数

        void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);
FSMC 使能函数,这里我们使用的类型是NORSRAM，所以使用上面的函数，如果是其他的类型的存储调用相应的使能函数。

## 软件设计、开发 ##
在工程中添加lcd.c 文件和lcd.h 文件，LCD相关的函数和定义都在这两个文件中。
编写LCD 用GPIO配置函数，初始化所有LCD相连的GPIO。

        void LCD_CtrlLinesConfig(void)
        {

            GPIO_InitTypeDef GPIO_InitStructure;

            /* Enable GPIOD, GPIOE, and AFIO clocks */
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | 
                                    RCC_AHB1Periph_GPIOE, ENABLE);

            /*-- GPIO Configuration ---*/
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15 |
                GPIO_Pin_0  | GPIO_Pin_1 |   GPIO_Pin_7 |
                GPIO_Pin_8  | GPIO_Pin_9 | GPIO_Pin_10 | 
                GPIO_Pin_11 | GPIO_Pin_4 | GPIO_Pin_5;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
                    GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 |
                    GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
            GPIO_Init(GPIOE, &GPIO_InitStructure);

            GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
            GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

            /*PD13(LCD_BL) configuration*/
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            GPIO_SetBits(GPIOD, GPIO_Pin_13);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            {
                int i;
                GPIO_ResetBits(GPIOD, GPIO_Pin_3);
                for (i = 0; i < 1000000; i++)
                {
                    ;
                }
                GPIO_SetBits(GPIOD, GPIO_Pin_3);
            }
        }

为LCD配置 FSMC

        void LCD_FSMCConfig(void)
        {
            FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
            FSMC_NORSRAMTimingInitTypeDef  p;

            /* Enable FSMC clock */
            RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

            /* FSMC_Bank1_NORSRAM_1 configuration */
            p.FSMC_AddressSetupTime = 5;
            p.FSMC_AddressHoldTime = 0;
            p.FSMC_DataSetupTime = 9;
            p.FSMC_BusTurnAroundDuration = 0;
            p.FSMC_CLKDivision = 0;
            p.FSMC_DataLatency = 0;
            p.FSMC_AccessMode = FSMC_AccessMode_A;

            FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
            FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
            FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	
            FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
            FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
            FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
            FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
            FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
            FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = \
                    FSMC_WaitSignalActive_BeforeWaitState;
            FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
            FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
            FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
            FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

            FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
            FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

            FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   

            /* Enable FSMC NOR/SRAM Bank1 */
            FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
        }

LCD使用的是FSMC NOR/SRAM Bank1，其所对应地址为0x60000000，宏定义如下：

        #define LCD_BASE           ((uint32_t)(0x60000000))
        
LCD的RS接地址A16，LCD使用的是 16 位数据宽度，所以 HADDR[0]并没有用到，只有 HADDR[25:1]是有效的，对应关
系变为：HADDR[25:1] --> FSMC[24:0]，相当于右移了一位，故CMD地址为0x60020000，宏定义如下：

        #define LCD_CMD_ADDR       ((uint32_t)(0x60020000))   
        
根据LCD的datasheet 设置如下宏定义：

        //RS=1 发送数据  命令的值或像数值
        #define LCD_CMD_REG        (*( uint16_t *)LCD_CMD_ADDR)
        #define LCD_RAM_REG        (*( uint16_t *)LCD_CMD_ADDR)
        
        //RS=0 设置地址（寄存器索引）
        #define LCD_IDX_REG        (*((__IO uint16_t *)LCD_BASE))   
       
宏定义LCD屏的点阵范围

        #define LCD_W     (240)
        #define LCD_H     (400)

向LCD寄存器写数据，LCD_Reg 寄存器ID，LCD_RegValue 寄存器内容

        void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
        {
            /* Write 16-bit Index, then Write Reg */
            LCD_IDX_REG = LCD_Reg;          //选择LCD寄存器地址

            /* Write 16-bit Reg */
            LCD_CMD_REG = LCD_RegValue;     //发送数据
        }
        
操作索引地址，设置像数点的颜色，REG_Code 像素点的颜色

        __inline void LCD_REG_Select(uint16_t REG_Code)
        {
            LCD_IDX_REG = REG_Code;
        }

配置LCD使其处于接收显示数据的状态

        __inline void LCD_WriteRAM_Prepare(void)
        {
            LCD_IDX_REG =  LCD_REG_DATA;
        }

向LCD显存写入显示数据
 
        __inline void LCD_WriteRAM(uint16_t RGB_Code)
        {
            LCD_RAM_REG = RGB_Code;
        }
        
LCD初始化配置，首先初始化GPIO管脚，初始化LCD的FSMC配置寄存器，然后按照LCD datasheet 的配置顺序初始化配置寄存器。
        void init_lcd(void)
        { 
            /*配置LCD 用到的GPIO */
            LCD_CtrlLinesConfig();

            /* Configure the FSMC Parallel interface */
            LCD_FSMCConfig();
            delay_ms(10); /*delay 10 ms*/

            LCD_WriteReg(0x001A,0x0002);
            LCD_WriteReg(0x001B,0x0088);

            //****VCOM offset**///
            LCD_WriteReg(0x0023,0x0000);
            LCD_WriteReg(0x0024,0x0064);
            LCD_WriteReg(0x0025,0x0015);
            LCD_WriteReg(0x002D,0x0003);
            delay_ms(10);

            //****OPON**//
            LCD_WriteReg(0x00E8,0x0060);

            //Power on Setting
            LCD_WriteReg(0x0018,0x0004);
            LCD_WriteReg(0x0019,0x0001);
            LCD_WriteReg(0x0001,0x0000);
            LCD_WriteReg(0x001F,0x0088);

            delay_ms(10);
            LCD_WriteReg(0x1F,0x80);

            delay_ms(10);
            LCD_WriteReg(0x1F,0x90);

            delay_ms(10);
            LCD_WriteReg(0x1F,0xD0);
            delay_ms(10);

            //262k/65k color selection
            LCD_WriteReg(0x17,0x05);

            //SET PANEL
            LCD_WriteReg(0x36,0x03);
            LCD_WriteReg(0x29,0x31);
            LCD_WriteReg(0x71,0x1A);

            //Gamma 2.2 Setting
            LCD_WriteReg(0x40,0x00);
            LCD_WriteReg(0x41,0x45);
            LCD_WriteReg(0x42,0x45);
            LCD_WriteReg(0x43,0x04);

            LCD_WriteReg(0x44,0x00);
            LCD_WriteReg(0x45,0x08);
            LCD_WriteReg(0x46,0x23);
            LCD_WriteReg(0x47,0x23);

            LCD_WriteReg(0x48,0x77);
            LCD_WriteReg(0x49,0x40);
            LCD_WriteReg(0x4A,0x04);
            LCD_WriteReg(0x4B,0x00);
            LCD_WriteReg(0x4C,0x88);
            LCD_WriteReg(0x4D,0x88);
            LCD_WriteReg(0x4E,0x88);

            LCD_WriteReg(0x02,0x00);
            LCD_WriteReg(0x03,0x00);

            LCD_WriteReg(0x04,0x00);
            LCD_WriteReg(0x05,0xEF);

            LCD_WriteReg(0x06,0x00);
            LCD_WriteReg(0x07,0x00);

            LCD_WriteReg(0x08,0x01);
            LCD_WriteReg(0x09,0x8F);


            //Display ON Setting
            LCD_WriteReg(0x28,0x38); 
            delay_ms(40);
            LCD_WriteReg(0x28,0x3F); 

            LCD_REG_Select(0x22);
        }


 开启LCD显示
 
        void LCD_DisplayOn(void)
        {
            /* Display On */
            LCD_WriteReg(0x07, 0x0173); /* 262K color and display ON */
        }

禁止LCD显示

        void LCD_DisplayOff(void)
        {
            /* Display Off */
            LCD_WriteReg(0x7, 0x0); 
        }
        
设置LCD显存当前的位置 Xpos  x坐标，Ypos  y坐标

        void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
        {
            LCD_WriteReg(0x02, ((Xpos>>8)&0xFF));
            LCD_WriteReg(0x03, (Xpos&0xFF));

            LCD_WriteReg(0x06, ((Ypos>>8)&0xFF));
            LCD_WriteReg(0x07, (Ypos&0xFF));
        }

在指定位置画一个点 x坐标, y坐标, Color RGB颜色值。

        void PutPixel(int16_t x, int16_t y, uint16_t Color)
        { 
            if(y < 0 || y > (LCD_H - 1) || x < 0 || x > (LCD_W - 1))
            {
                return;
            }

            //设置光标位置 
            LCD_SetCursor(x,y);
            LCD_WriteRAM_Prepare(); 
            LCD_WriteRAM(Color);
        }

清屏函数
        void LCD_Clear(uint16_t Color)
        {
            uint32_t index = 0;

            LCD_SetCursor(0x00, 0x00); 
            LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
            for(index = 0; index < LCD_W * LCD_H; index++)
            {
                LCD_RAM_REG = Color;
            }  
        }
        
编写main函数进行测试

        int main(void)
        {

            init_delay();

            init_lcd();	

            while(1)
            {
                LCD_Clear(Red);
                delay_ms(1000);

                LCD_Clear(White);
                delay_ms(1000);

            }
        }
        

## 软件下载、测试验证 ##


