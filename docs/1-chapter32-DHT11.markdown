# DHT11 实验 #

芯片内部有温度传感器，有兴趣的同学可以尝试编程读出温度值，本次实验我们使用的是板载的DTH11温湿度传感器，每隔1s将读取的温湿度值打印在串口终端和LCD上。

## 实验原理与基础知识 ##
DHT11 是一款湿温度一体化的数字传感器。该传感器包括一个电阻式测湿元件和一个 NTC测温元件，并与一个高性能 8 位单片机相连接。通过单片机等微处理器简单的电路连接就能够实时的采集本地湿度和温度。 DHT11 与单片机之间能采用简单的单总线进行通信，仅仅需要一个 I/O 口。传感器内部湿度和温度数据 40Bit 的数据一次性传给单片机，数据采用校验和方式进行校验，有效的保证数据传输的准确性。

**技术参数**

 * 供电电压： 3.3~5.5V DC
 
 * 输 出： 单总线数字信号
 
 * 测量范围： 湿度20-90%RH， 温度0~50℃
 
 * 测量精度： 湿度+-5%RH， 温度+-2℃
 
  * --- 分 辨 率： 湿度1%RH， 温度1℃
 
  * --- 互 换 性： 可完全互换 ，
  
  * ---  长期稳定性： <±1%RH/年
    
DHT11 数字湿温度传感器采用单总线数据格式。即，单个数据引脚端口完成输入输出双向传输。其数据包由 5Byte（40Bit）组成。数据分小数部分和整数部分，一次完整的数据传输为40bit，高位先出。DHT11 的数据格式为：8bit 湿度整数数据+8bit 湿度小数数据+8bit 温度整数数据+8bit 温度小数数据+8bit 校验和(前四个字节相加)。

用户MCU发送一次开始信号后,DHT11从低功耗模式转换到高速模式,等待主机开始信号结束后,DHT11发送响应信号,送出40bit的数据,并触发一次信号采集,用户可选择读取部分数据.从模式下,DHT11接收到开始信号触发一次温湿度采集,如果没有接收到主机发送开始信号,DHT11不会主动进行温湿度采集.采集数据后转换到低速模式。

DHT11 的传输时序

总线空闲状态为高电平,主机把总线拉低等待DHT11响应,主机把总线拉低必须大于18毫秒,保证DHT11能检测到起始信号。DHT11接收到主机的开始信号后,等待主机开始信号结束,然后发送80us低电平响应信号.主机发送开始信号结束后,延时等待20-40us后,读取DHT11的响应信号,主机发送开始信号后,可以切换到输入模式,或者输出高电平均可,总线由上拉电阻拉高。

![](img/chapter32/1.1.1.png) 

总线为低电平,说明DHT11发送响应信号,DHT11发送响应信号后,再把总线拉高80us,准备发送数据,每一bit数据都以50us低电平时隙开始,高电平的长短定了数据位是0还是1.格式见下面图示.如果读取响应信号为高电平,则DHT11没有响应,请检查线路是否连接正常.当最后一bit数据传送完毕后，DHT11拉低总线50us,随后总线由上拉电阻拉高进入空闲状态。数字0信号表示方法如图4所示

![](img/chapter32/1.1.2.png) 

数字1信号表示方法.

![](img/chapter32/1.1.3.png) 

## 电路设计 ##

![](img/chapter0/adc_sch.png) 

## 库函数和数据结构 ##


## 软件设计、开发 ##
在工程中添加 dht11.c 文件和dht11.h 文件，所有 DHT11 相关的驱动代码和定义都在这两个文件中。

从原理图上可知STM32F407是通过GPB0与DHT11相连，我们编写管脚初始化函数

        void init_GPB0(void)
        
一总线传输，GPB0根据时序需要IO输入输出的方向，编写如下宏定义：

        //IO输入输出设置
        #define DHT11_IO_IN()  {GPIOB->MODER&=~(3<<(0*2));GPIOB->MODER|=0<<0*2;}	//PB0输入模式
        #define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(0*2));GPIOB->MODER|=1<<0*2;} 	//PB0输出模式

编写GPB0输出函数和读取函数：

        void DHT11_OUT(u8 value)
        {
            if(1==value)
                GPIO_SetBits(GPIOB,GPIO_Pin_0); 
            else
                GPIO_ResetBits(GPIOB,GPIO_Pin_0);

        }

        u8 DHT11_IN(void)
        {
            return 0x01 & GPIOB->IDR;

        }
        
编写DHT11复位函数 和 响应检查函数:

        void DHT11_Rst(void)	   
        {                 
            DHT11_IO_OUT(); 	//SET OUTPUT
            DHT11_OUT(0); 		//拉低
            delay_ms(20);    	//拉低至少18ms
            DHT11_OUT(1); 		//拉高 
            delay_us(30);     	//主机拉高20~40us
        }

编写读一个bit 读一个字节，读整个40位的数据

        u8 DHT11_Read_Bit(void) 			 
        {
            u8 retry=0;
            while(DHT11_IN()&&retry<100)//等待变为低电平
            {
                retry++;
                delay_us(1);
            }
            retry=0;
            while(!DHT11_IN()&&retry<100)//等待变高电平
            {
                retry++;
                delay_us(1);
            }
            delay_us(40);//等待40us
            if(DHT11_IN())
                return 1;
            else 
            return 0;		   
        }

        u8 DHT11_Read_Byte(void)    
        {        
            u8 i,dat;
            dat=0;
            for (i=0;i<8;i++) 
            {
                dat<<=1; 
                dat|=DHT11_Read_Bit();
            }						    
            return dat;
        }

        u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
        {        
            u8 buf[5];
            u8 i;
            DHT11_Rst();
            if(DHT11_Check()==0)
            {
                for(i=0;i<5;i++)//读取40位数据
                {
                    buf[i]=DHT11_Read_Byte();
                }
                if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
                {
                    *humi=buf[0];
                    *temp=buf[2];
                }
            }
            else 
                return 1;
            return 0;	    
        }
        
编写main函数进行测试

        int main()
        {
            u8 temperature; 

            u8 humidity;

            DrawMode Mode;

            Mode.FColor = 0xF800;
            Mode.BColor = 0x001F;
            Mode.Mode = 0;

            init_delay();

            init_uart();

            init_DHT11();

            init_lcd();

            while(1)
            {

                DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值

                printf("DHT11  temperature: %d  humidity: %d\r\n",temperature,humidity);	

                LCD_DisplayNum(0, 80, temperature, &Mode);

                LCD_DisplayNum(0, 100, humidity, &Mode);

                delay_ms(1000);
            }


        }
        
## 软件下载、测试验证 ##


