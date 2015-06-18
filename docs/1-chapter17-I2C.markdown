# IIC实验 #

IIC（Inter－Integrated Circuit）总线是由PHILIPS公司开发的两线式串行总线，用于连接微控制器及其外围设备。是微电子通信控制领域广泛采用的一种总线标准。

## 实验原理与基础知识 ##

Elink407系列开发板通过IIC总线外接EEPROM设备（AT24C64），即带电可擦写可编程只读存储器，它是用户可更改的只读存储器，其可通过高于普通电压的作用来擦除和重编程（重写）。EEPROM不需从计算机中取出即可修改，EEPROM是一种特殊形式的闪存，其应用通常是在接口卡等嵌入式系统中，用来存放硬件设置数据。

AT24C64是低工作电压的64Kbit位串行电可擦除只读存储器，内部组织为8192个字节，每个字节8位，该芯片被广泛应用于低电压及低功耗的工商业领域。

AT25C64使用两线串行接口和微处理器进行通信，遵循IIC总线协议。

IIC协议术语：

发送器：发送数据到总线的器件；

接收器：从总线接收数据的器件；

主机：启动数据传送并产生时钟信号的设备；

从机：被主机寻址的器件；

多主机：同时有多于一个主机尝试控制总线但不破坏传输；

主模式：用I2CNDAT支持自动字节计数的模式；位I2CRM,I2CSTT,I2CSTP控制数据的接收和发送；

从模式：发送和接收操作都是由I2C模块自动控制的；

仲裁：是一个在有多个主机同时尝试控制总线但只允许其中一个控制总线并使传输不被破坏的过程；

同步：两个或多个器件同步时钟信号的过程 。

IIC总线——起始和停止信号：

SCL 线是高电平时，SDA 线从高电平向低电平切换，这个情况表示起始条件；

SCL 线是高电平时，SDA 线由低电平向高电平切换，这个情况表示停止条件。

起始和停止条件一般由主机产生，总线在起始条件后被认为处于忙的状态起始和停止条件起始和停止条件，在停止条件的某段时间后总线被认为再次处于空闲状态。如果产生重复起始条件而不产生停止条件，总线会一直处于忙的状态，此时的起始条件（S）和重复起始条件（Sr）在功能上是一样的。

IIC总线——数据传输格式：

字节格式

发送到SDA 线上的每个字节必须为8 位，每次传输可以发送的字节数量不受限制。每个字节后必须跟一个响应位。首先传输的是数据的最高位（MSB），如果从机要完成一些其他功能后（例如一个内部中断服务程序）才能接收或发送下一个完整的数据字节，可以使时钟线SCL 保持低电平，迫使主机进入等待状态，当从机准备好接收下一个数据字节并释放时钟线SCL 后数据传输继续。

应答响应

数据传输必须带响应，相关的响应时钟脉冲由主机产生。在响应的时钟脉冲期间发送器释放SDA 线（高）。

在响应的时钟脉冲期间，接收器必须将SDA 线拉低，使它在这个时钟脉冲的高电平期间保持稳定的低电平。

如果从机接收器响应了从机地址，但是在传输了一段时间后不能接收更多数据字节，主机必须再一次终止传输。这个情况用从机在第一个字节后没有产生响应来表示。从机使数据线保持高电平，主机产生一个停止或重复起始条件。

如果传输中有主机接收器，它必须通过在从机发出的最后一个字节时产生一个响应，向从机发送器通知数据结束。从机发送器必须释放数据线，允许主机产生一个停止或重复起始条件。

## 电路设计 ##

本案例使用的IIC器件AT24C64封装类型为SOP8，其数据线SCL和时钟线SDA分别与stm32407的PB6、PB7相连接。

![](img/chapter0/eeprom_sch.png) 

## 库函数和数据结构 ##
IIC总线基本配置直接相关的几个固件库函数，这些函数的定义主要分布在stm32f4xx_i2c.h 和 stm32f4xx_i2c.c 文件中。

发送起始信号：

        I2C_GenerateSTART(I2C1, ENABLE);
        
发送停止信号：

        I2C_GenerateSTOP(I2C1, ENABLE);
        
读取I2C指定寄存器到值：

        I2C_ReadRegister(I2C1, I2C_Register)；
        
获取状态标志：

        I2C_GetFlagStatus(I2C1, I2C_FLAG)
        
开启I2C响应：

        I2C_AcknowledgeConfig(I2C1, ENABLE)
        
## 软件设计、开发 ##
在工程中添加iic.c 文件和iic.h 文件，IIC相关的函数和定义都在这两个文件中。

1)  IIC的GPIO配置。

        void I2C_Configuration(void)
        {
            I2C_InitTypeDef  I2C_InitStructure;
            GPIO_InitTypeDef  GPIO_InitStructure; 

            RCC_AHB1PeriphClockCmd(I2C_SDA_GPIO_CLK,ENABLE);
            RCC_AHB1PeriphClockCmd(I2C_SCL_GPIO_CLK,ENABLE);

            RCC_APB1PeriphClockCmd(I2C_CLK,ENABLE);

            GPIO_PinAFConfig(I2C_SDA_GPIO_PORT, I2C_SDA_SOURCE, I2C_SDA_AF);
            GPIO_PinAFConfig(I2C_SCL_GPIO_PORT, I2C_SCL_SOURCE, I2C_SCL_AF);

            GPIO_InitStructure.GPIO_Pin =  I2C_SDA_PIN | I2C_SCL_PIN;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //PB6和PB7设置为开漏输出
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            I2C_DeInit(I2C);
            I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
            I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
            I2C_InitStructure.I2C_OwnAddress1 = 0x0;//当I2C出于从模式时,自身的地址
            I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
            I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
            I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
            I2C_Init(I2C, &I2C_InitStructure);

            I2C_Cmd(I2C, ENABLE);
            I2C_AcknowledgeConfig(I2C, ENABLE);
        }
        
        
2)  读数据操作
        uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t *buf,uint16_t num)
        {
            if(num==0){
                return 1;
            }
            while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

            I2C_AcknowledgeConfig(I2Cx, ENABLE);

            I2C_GenerateSTART(I2Cx, ENABLE);
            while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

            I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
            while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

            //I2C_SendData(I2Cx, addr);
            I2C_SendData(I2C1, addr/256);  //高位
            I2C_SendData(I2C1, addr%256);  //低位
            while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

            I2C_GenerateSTART(I2Cx, ENABLE);
            while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

            I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
            while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
            {}

            while (num)
            {
                if(num==1)
                {
                    I2C_AcknowledgeConfig(I2Cx, DISABLE);
                    I2C_GenerateSTOP(I2Cx, ENABLE);
                }

                while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
                {}
                *buf = I2C_ReceiveData(I2Cx);
                buf++;
                /* Decrement the read bytes counter */
                num--;
            }

            I2C_AcknowledgeConfig(I2Cx, ENABLE);

            return 0;
        }

3)  写数据操作

        //写一个字节
        uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t value)
        {
            //判断BUSY是否为1,BUSY=1总线处于忙状态,如果BUSY=1则等待一直等到BUSY=0
            while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)){
                if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback();
            }

              /* Send STRAT condition */
            I2C_GenerateSTART(I2C1, ENABLE);

            /* Test on EV5 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  

            /* Send EEPROM address for write */
            I2C_Send7bitAddress(I2C1, I2C_Addr, I2C_Direction_Transmitter);

            /* Test on EV6 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

            /* Send the EEPROM's internal address to write to */  
            I2C_SendData(I2C1, addr/256);  //高位
            I2C_SendData(I2C1, addr%256);  //低位
            /* Test on EV8 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

            /* Send the byte to be written */
            I2C_SendData(I2C1, value); 
            /* Test on EV8 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

            /* Send STOP condition */
            I2C_GenerateSTOP(I2C1, ENABLE);

            I2C_EE_WaitEepromStandbyState();   

            I2C_delay(1000);

            return 0;
        }


        //写多个字节
        uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t *buf,uint16_t num)
        {
            uint8_t err=0;
            while(num--)
            {
                if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf))
                {
                    err++;
                }
                buf++;
            }
            if(err)
                return 1;
            else 
                return 0;	
        }
        
        
4）擦除数据

        void I2C_Erase(void)
        {
            uint16_t i;
            for (i = 0; i <8*1024; i++)
            {
                I2C_WriteOneByte(I2C,ADDR_24L64,i,0xFF);
            }
        }


5) 编写main函数

        int main(void)
        {
            uint16_t Addr;
            uint8_t WriteBuffer[256]={0};
            uint8_t ReadBuffer[256]={0};
            uart_config();
            printf("\r\n***********************r\n");

            I2C_Configuration();

            for(Addr=0; Addr<256; Addr++){
                WriteBuffer[Addr]=Addr+100;	 /* 填充WriteBuffer */
            }

            //全部擦出，每个字节置0xff
            //I2C_Erase();
            //printf("Erase ok\r\n");

            /* 开始向EEPROM写数据 */
            printf("\r\n EEPROM 24C64 Write Test \r\n");

            for(Addr=0;Addr<sizeof(WriteBuffer);Addr++){
                printf("%d ",WriteBuffer[Addr]);
            }
            printf("\r\n");

            I2C_Write(I2C,ADDR_24L64,0x0,WriteBuffer,sizeof(WriteBuffer) );
            printf("\r\n EEPROM 24C64 Write Test OK \r\n");

            /* EEPROM读数据 */
            printf("\r\n EEPROM 24C64 Read Test \r\n");
            I2C_Read(I2C,ADDR_24L64,0x0,ReadBuffer,sizeof(ReadBuffer) );

            for(Addr=0;Addr<sizeof(ReadBuffer);Addr++){
                printf("%d ",ReadBuffer[Addr]);
            }
            printf("\r\n");

            if(strcmp((const char *)ReadBuffer,(const char *)WriteBuffer)==0){
                printf("\r\n EEPROM 24C64 Read Test ok \r\n");
            }

            while (1)
            {
            }
        }

## 软件下载、测试验证 ##


