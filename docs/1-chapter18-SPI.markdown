# SPI实验 #

串行外设接口SPI（Serial Peripheral Interface）是一种高速的、全双工、同步的通信总线，SPI总线系统可直接与各个厂家生产的多种标准外围器件直接连接，该接口一般使用4条线：串行时钟线（SCLK）、主机输入/从机输出数据线MISO、主机输出/从机输入数据线MOSI和低电平有效的从机选择线NSS。

## 实验原理与基础知识 ##

ELink407系列开发板通过SPI外接flash设备（SST25VF016B）和网络设备（EMW1062），本案例使用flash，需将J21的1、2引脚用条线帽短接。

接口信号

（1）MOSI – 主器件数据输出，从器件数据输入

（2）MISO – 主器件数据输入，从器件数据输出

（3）SCLK –时钟信号，由主器件产生，最大为fPCLK/2，从模式频率最大为fCPU/2

（4）NSS – 从器件使能信号，由主器件控制，有的SPI器件会标注为CS(Chip select)

在点对点的通信中，SPI接口不需要进行寻址操作，且为全双工通信，显得简单高效。多个从器件硬件连接示意图在多个从器件的系统中，每个从器件需要独立的使能信号，硬件上比I2C系统要稍微复杂一些。

SPI接口在内部硬件实际上是两个简单的移位寄存器，传输的数据为8位，在主器件产生的从器件使能信号和移位脉冲下，按位传输，高位在前，低位在后。如下图所示，在SCLK的上升沿上数据改变，同时一位数据被存入移位寄存器。

## 电路设计 ##

本案例使用的SPI器件SST25VF016B封装类型为SOP8，引脚与处理器GPIO连接的对应关系如下：

PA15——SPI1_CS

PA5——SPI1_SCK

PB4——SPI1_MISO

PB5——SPI1_MOSI

![](img/chapter0/spi_sch.png) 

## 库函数和数据结构 ##
SPI总线基本配置直接相关的几个固件库函数，这些函数的定义主要分布在stm32f4xx_spi.h 和 stm32f4xx_spi.c 文件中。

使能SPI1时钟总线：

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
        
定义片选宏函数，信号为低时生效：

        #define SST25V_CS_LOW()               GPIO_ResetBits(GPIOA, GPIO_Pin_15)
        #define SST25V_CS_HIGH()              GPIO_SetBits(GPIOA, GPIO_Pin_15)
        
等待SPI1的发送缓冲区为空：

        SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE);
        
发送数据字节：

        SPI_SendData(SPI1, byte);
        
等待SPI1设备完成接收数据：

        SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE)； 
        
接受读到的数据：

        SPI_ReceiveData(SPI1)；
        
开启SPI1：

        SPI_Cmd(SPI1, ENABLE); 

## 软件设计、开发 ##
在工程中添加spiflash.c 文件和spiflash.h 文件，SPI flash相关的函数和定义都在这两个文件中。

1)  时钟总线配置

        void RCC_Configuration(void)
        {
	       RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	       RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
        }

2） SPI的GPIO配置。

        void GPIO_Configuration(void)
        {	
            //PA5--SPI1_SCK  PB4--SPI1--MISO PB5--SPI1--MOSI  PA15--SPI1CS
            GPIO_InitTypeDef GPIO_InitStructure;
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
            GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
            GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);

            //PA15--SPI1_CS
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
            //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
        }
        
3)  配置SPI1总线为master模式

        void SPI_configuration()  //SPI1 configuration
        {
            SPI_InitTypeDef  SPI_InitStructure;

            SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
            SPI_InitStructure.SPI_Mode = SPI_Mode_Master;     
            SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;  
            SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI1, &SPI_InitStructure);
            SPI_Cmd(SPI1, ENABLE);  //Enable SPI1
        }
        
4)  读取flash数据

        //SPI发送一个字节数据
        u8 SPI_Flash_SendByte(u8 byte) 
        {
            while(SPI_GetFlagStatus(SPI1, SPI_FLAG_TXE) == RESET);     //等待SPI1的Tx buffer为空
            SPI_SendData(SPI1, byte);                                  //发送数据字节byte
            while(SPI_GetFlagStatus(SPI1, SPI_FLAG_RXNE) == RESET);    //等待SPI1完成接收数据
            return SPI_ReceiveData(SPI1);                              //返回读到的数据
        }
        
        //从ReadAddr读取一个字节数据
        u8 SST25V_ByteRead(u32 ReadAddr) 
        {
            u8 Temp = 0;
            SST25V_CS_LOW();                                  
            SPI_Flash_SendByte(Read_Data);                    
            SPI_Flash_SendByte((ReadAddr & 0xFF0000) >> 16);  //24位地址
            SPI_Flash_SendByte((ReadAddr& 0xFF00) >> 8);
            SPI_Flash_SendByte(ReadAddr & 0xFF);

            Temp = SPI_Flash_ReceiveByte();                   
            SST25V_CS_HIGH();                      
            return Temp;                       
        }

        //从ReadAddr读取NumByteToRead字节数据
        void SST25V_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
        {
            SST25V_CS_LOW();                                  
            SPI_Flash_SendByte(Read_Data);                 
            SPI_Flash_SendByte((ReadAddr & 0xFF0000) >> 16);  
            SPI_Flash_SendByte((ReadAddr& 0xFF00) >> 8);
            SPI_Flash_SendByte(ReadAddr & 0xFF);
            while(NumByteToRead--)             
            {
                *pBuffer = SPI_Flash_ReceiveByte(); 
                pBuffer++;                      
            }
            SST25V_CS_HIGH();     
        }
        
5） 写数据到flash

        //写入一个字节数据
        void SST25V_ByteWrite(u8 Byte, u32 WriteAddr)  
        {
            SST25V_WriteEnable();                                
            SST25V_CS_LOW();                                    
            SPI_Flash_SendByte(Byte_Program);                   
            SPI_Flash_SendByte((WriteAddr & 0xFF0000) >> 16);   
            SPI_Flash_SendByte((WriteAddr & 0xFF00) >> 8);
            SPI_Flash_SendByte(WriteAddr & 0xFF);

            SPI_Flash_SendByte(Byte);
            SST25V_CS_HIGH();
            SST25V_WaitForWriteEnd(); 
        }
        //以WriteAddr为起始地址,写入ByteLength个Byte数据(相同的Byte)
        void SST25V_WriteBytes(u8 Byte, u32 WriteAddr,u32 ByteLength)  
        {
            while(ByteLength--)
            {
                SST25V_ByteWrite(Byte,WriteAddr);
                WriteAddr++;  
            }
        }
        
        //等待写操作完成
        void SST25V_WaitForWriteEnd(void)     
        {
            u8 FLASH_Status = 0;
            SST25V_CS_LOW();                            
            do
            {
                FLASH_Status = SST25V_ReadStatusRegister(); 

            } while((FLASH_Status & 0x01) == SET);  
            SST25V_CS_HIGH();     
        }
        
6) 格式化flash

        void SST25V_ChipErase(void)            //擦出整个芯片数据空间
        {
            SST25V_WriteEnable();             
            SST25V_CS_LOW();       
            SPI_Flash_SendByte(ChipErace);  
            SST25V_CS_HIGH();    
            SST25V_WaitForWriteEnd();  
        }
        
7) 编写main函数

        int main()
        {
            uart_config();
            printf("test usart!\r\n");

            Setup_System();  
            SST25V_Init();  
            SST25V_ChipErase();

            //test flash 1,擦出读测试
            DataByte = SST25V_ByteRead(0x000000);    
            printf("data1=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x000010);  
            printf("data2=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x000100);  
            printf("data4=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x001000); 
            printf("data5=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x010000); 
            printf("data6=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x100000); 
            printf("data7=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x600000); 
            printf("data8=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xF00000); 
            printf("data9=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xFFFFFF); 
            printf("data10=%x\r\n",DataByte);

            //test flash 2，向地址0xfff000写入0x1000个字符0x72
            SST25V_AAI_WriteBytes(0x72, 0xFFF000,0x001000);

            DataByte = SST25V_ByteRead(0x000000); 
            printf("data11=%x\r\n",DataByte);	
            DataByte = SST25V_ByteRead(0x000001); 
            printf("data12=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0x000002); 
            printf("data12=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xFFF000); 
            printf("data13=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xFFF0EE); 
            printf("data14=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xFFFEEE); 
            printf("data15=%x\r\n",DataByte);
            DataByte = SST25V_ByteRead(0xFFFFFF); 

            //test flash 3，向地址0xfff000写字符串
            SST25V_ChipErase();
            SST25V_AAI_BufferProgram(Tx_Buffer,0xFFF000,BufferSize);
            printf("tx_buffer=%s\r\n",Tx_Buffer);
            SST25V_BufferRead(Rx_Buffer,0xFFF000,BufferSize); 
            printf("rx_buffer=%s\r\n",Rx_Buffer);

            DataByte = SST25V_ByteRead(0xFFEEEE); 
            printf("data16=%d\r\n",DataByte);
            return 0;
        }

## 软件下载、测试验证 ##


