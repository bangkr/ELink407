# CRC 实验 #

## 实验原理与基础知识 ##

CRC（循环冗余校验）计算单元使用一个固定的多项式发生器从一个32 位的数据字中产生 CRC 码。

在众多的应用中，基于CRC 的技术还常用来验证数据传输或存储的完整性。这些技术提供了验证Flash 完整性的方法。

CRC 计算单元有助于在运行期间计算软件的签名，并将该签名与链接时生成并存储在指定存储单元的参考签名加以比较。

![](img/chapter30/1.1.1.png) 

CRC 计算单元主要由单个32 位数据寄存器组成，该寄存器：

● 用作输入寄存器，向CRC 计算器中输入新数据 （向寄存器写入数据时）

● 可保存之前的CRC 计算结果 

对数据寄存器的每个写操作都会把当前新输入的数值和之前生成在数据寄存器中的CRC 值再做一次CRC 计算 （CRC 计算针对整个32 位数据字完成，而非逐字节进行）。

CRC 计算的时候，写操作被阻塞，因此允许执行背靠背写访问或连续的写读访问。使用CRC_CR 寄存器中的RESET 控制位即可将CRC 计算器复位为0xFFFF FFFF。此操作不影响CRC_IDR 寄存器的内容。

CRC 寄存器

CRC 计算单元包含两个数据寄存器和一个控制寄存器。CRC 寄存器必须按字（32 位）访问。

**数据寄存器(CRC_DR)       Data register**

偏移地址：0x00

![](img/chapter30/1.1.2.png) 

**独立数据寄存器(CRC_IDR)  Independent data register**

偏移地址：0x04

![](img/chapter30/1.1.3.png) 

**控制寄存器(CRC_CR)       Control register**

偏移地址：0x08
        
![](img/chapter30/1.1.4.png) 

## 电路设计 ##

## 库函数和数据结构 ##
相关的操作在文件 stm32f4xx_crc.c 和对应的头文件 stm32f4xx_crc.h 中

        void CRC_ResetDR(void) 
        
用于复位 CRC 数据寄存器 

        uint32_t CRC_CalcCRC(uint32_t Data) 
        
用于计算 32 位数的 CRC,参数Data  输入的 32 位数,返回 32 位数的 CRC 计算结果 

        uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength) 
        
用于计算多个 32 位数的 CRC。参数：  pBuffer[]  缓冲区地址   BufferLength  缓冲区长度 ,返回一个 32 位的 CRC 计算结果。 

        uint32_t CRC_GetCRC(void) 
        
返回当前的 CRC 数值 

        void CRC_SetIDRegister(uint8_t IDValue) 
        
存入一个 8 位数据到独立数据寄存器。 

        uint8_t CRC_GetIDRegister(void) 
        
从独立数据寄存器读出 8 位数据。

## 软件设计、开发 ##


## 软件下载、测试验证 ##


