# 触摸屏实验 #
每一代人机交互界面改变都是用户体验全面提升而且在计算机领域都是一件里程碑的事件，之前计算机开始之初的时代，人们通过打孔纸带，完成跟计算机进行的交互，可以想象读写这样的二进制信息是何等的艰辛，后来人们发明了键盘和显示器，人们通过文本字符和计算机进行交互，之后发明了鼠标，可以通过图形用户界面进行交互，计算机不再只是专业人员的工具，通过视窗界面普通用户也能使用计算机。触摸，多点触控已成为当前人机交互的主流技术，利用这种技术，我们用户只要用手指轻轻地指碰计算机显示器上的图符或文字就能实现对主机操作，从而使人机交互更为直截了当，这种技术极大方便了那些不懂电脑操作的用户。这种人机交互方式。它赋予了多媒体以崭新的面貌，是极富吸引力的全新多媒体交互设备。ELink407开发板LCD上面有电阻触摸屏，可以像平板一样通过触摸方式进行人机交互，本案例我们研究触摸的驱动和结合LCD屏进行使用。

## 实验原理与基础知识 ##
电阻式触摸屏是一种传感器，基本上是薄膜加上玻璃的结构，薄膜和玻璃相邻的一面上均涂有ITO（纳米铟锡金属氧化物）涂层，ITO具有很好的导电性和透明性。当触摸操作时，薄膜下层的ITO会接触到玻璃上层的ITO，经由感应器传出相应的电信号，经过转换电路送到处理器，通过运算转化为屏幕上的X、Y值，而完成点选的动作，并呈现在屏幕上。

![](img/chapter27/27_1.jpg) 

电阻触摸屏的工作原理主要是通过压力感应原理来实现对屏幕内容的操作和控制的，这种触摸屏屏体部分是一块与显示器表面非常配合的多层复合薄膜，其中第一层为玻璃或有机玻璃底层，第二层为隔层，第三层为多元树脂表层，表面还涂有一层透明的导电层，上面再盖有一层外表面经硬化处理、光滑防刮的塑料层。在多元脂表层表面的传导层及玻璃层感应器是被许多微小的隔层所分隔电流通过表层，轻触表层压下时，接触到底层，控制器同时从四个角读出相称的电流及计算手指位置的距离。这种触摸屏利用两层高透明的导电层组成触摸屏，两层之间距离仅为2.5微米。当手指触摸屏幕时，平常相互绝缘的两层导电层就在触摸点位置有了一个接触，因其中一面导电层接通Y轴方向的5V均匀电压场，使得侦测层的电压由零变为非零，控制器侦测到这个接通后，进行A/D转换，并将得到的电压值与5V相比，即可得触摸点的Y轴坐标，同理得出X轴的坐标，这就是所有电阻技术触摸屏共同的最基本原理。 

![](img/chapter27/27_2.jpg) 

触摸屏包含上下叠合的两个透明层，四线和八线触摸屏由两层具有相同表面电阻的透明阻性材料组成，五线和七线触摸屏由一个阻性层和一个导电层组成，通常还要用一种弹性材料来将两层隔开。当触摸屏表面受到的压力（如通过笔尖或手指进行按压）足够大时，顶层与底层之间会产生接触。所有的电阻式触摸屏都采用分压器原理来产生代表X坐标和Y坐标的电压。如图3，分压器是通过将两个电阻进行串联来实现的。上面的电阻（R1）连接正参考电压（VREF），下面的电阻（R2）接地。两个电阻连接点处的电压测量值与下面那个电阻的阻值成正比。 


![](img/chapter27/27_3.jpg) 

为了在电阻式触摸屏上的特定方向测量一个坐标，需要对一个阻性层进行偏置：将它的一边接VREF，另一边接地。同时，将未偏置的那一层连接到一个ADC的高阻抗输入端。当触摸屏上的压力足够大，使两层之间发生接触时，电阻性表面被分隔为两个电阻。它们的阻值与触摸点到偏置边缘的距离成正比。触摸点与接地边之间的电阻相当于分压器中下面的那个电阻。因此，在未偏置层上测得的电压与触摸点到接地边之间的距离成正比

![](img/chapter27/27_4.jpg) 

## 电路设计 ##

![](img/chapter0/lcd_sch.png) 

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

        I2C_AcknowledgeConfig(I2C1, ENABLE）
        

## 软件设计、开发 ##

*1 编写触摸屏驱动，在工程中添加iic.c 文件和iic.h 文件，IIC相关的函数和定义都在这两个文件中。*

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

            I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	        /* Send the byte to be written */
	        I2C_SendData(I2C1, value); 
            
            /* Test on EV8 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

            /* Send the byte to be written */
            I2C_SendData(I2C1, value); 
            /* Test on EV8 and clear it */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

            /* Send STOP condition */
            I2C_GenerateSTOP(I2C1, ENABLE);

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
        
*2. 进行采样，采集5个已知坐标点的阻值信息*

	/* Clear the LCD */ 
	LCD_Clear(Blue);
	get_sample (&cal, 0, 50,        50,        "Top left");
	
	get_sample (&cal, 1, XRES - 50, 50,        "Top right");
	
	get_sample (&cal, 2, XRES - 50, YRES - 50, "Bot right");
	
	get_sample (&cal, 3, 50,        YRES - 50, "Bot left");
	
	get_sample (&cal, 4, XRES / 2,  YRES / 2,  "Center")
	
	
	
	
	static void get_sample (calibration_t *cal,
			int index, int x, int y, char *name)
	{
		static int last_x = -1, last_y;
		tp_point_t point;
		Tp_Put_Cross(x, y,  0xFFFE);
	
		while(1){		
			if (pen_flag){
				TpGetXY(&point);
				pen_flag = 0;
				break;
			}
		} 
	
	
		//getxy (&cal->x [index], &cal->y [index]);
		cal->x[index] = point.x;
		cal->y[index] = point.y;
	
		Tp_Put_Cross(x, y,  0xFFFE);
		//	Delay (300);
	
		last_x = cal->xfb [index] = x;
		last_y = cal->yfb [index] = y;
	
		printf("%s : LCD X = %4d Y = %4d\n\r", name, cal->xfb [index], cal->yfb [index]);
		printf("%s : X = %4d Y = %4d\n\r", name, cal->x [index], cal->y [index]);
	}
	        
*3. 触摸屏校准计算*
        
	int Tp_Calibration(calibration_t *cal) 
	{
		int j;
		float n, x, y, x2, y2, xy, z, zx, zy;
		float det, a, b, c, e, f, i;
		float scaling = 65536.0;
	
	  	// Get sums for matrix
		n = x = y = x2 = y2 = xy = 0.0;
		for(j=0;j<5;j++) {
			printf("%f,%f\r\n",cal->x[j]*1.0,cal->y[j]*1.0);
			n += (float)1.0;
			x += (float)cal->x[j];
			y += (float)cal->y[j];
			x2 += (float)(cal->x[j]*cal->x[j]);
			y2 += (float)(cal->y[j]*cal->y[j]);
			xy += (float)(cal->x[j]*cal->y[j]);
		}
	
	  	// Get determinant of matrix -- check if determinant is too small
		det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
		if(det < (float)0.1 && det > (float)-0.1) {
			printf("ts_calibrate: determinant is too small -- %f\n",det);
			return 0;
		}
	
	  	// Get elements of inverse matrix
		a = (x2*y2 - xy*xy)/det;
		b = (xy*y - x*y2)/det;
		c = (x*xy - y*x2)/det;
		e = (n*y2 - y*y)/det;
		f = (x*y - n*xy)/det;
		i = (n*x2 - x*x)/det;
	
		// Get sums for x calibration
		z = zx = zy = 0;
		for(j=0;j<5;j++) {
			z += (float)cal->xfb[j];
			zx += (float)(cal->xfb[j]*cal->x[j]);
			zy += (float)(cal->xfb[j]*cal->y[j]);
		}
	
		// Now multiply out to get the calibration for framebuffer x coord
		cal->a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
		cal->a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
		cal->a[2] = (int)((c*z + f*zx + i*zy)*(scaling));
	
		printf("%f %f %f\n",(a*z + b*zx + c*zy),
					(b*z + e*zx + f*zy),
					(c*z + f*zx + i*zy));
	
		// Get sums for y calibration
		z = zx = zy = 0;
		for(j=0;j<5;j++) {
			z += (float)cal->yfb[j];
			zx += (float)(cal->yfb[j]*cal->x[j]);
			zy += (float)(cal->yfb[j]*cal->y[j]);
		}
	
		// Now multiply out to get the calibration for framebuffer y coord
		cal->a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
		cal->a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
		cal->a[5] = (int)((c*z + f*zx + i*zy)*(scaling));
	
		printf("%f %f %f\n",(a*z + b*zx + c*zy),
					(b*z + e*zx + f*zy),
					(c*z + f*zx + i*zy));
	
		// If we got here, we're OK, so assign scaling to a[6] and return
		cal->a[6] = (int)scaling;
		return 1;
	}


*4. 触摸屏测试函数*

	int ts_test(void)
	{
		calibration_t cal;
		tp_point_t point;
		int cal_fd;
		char cal_buffer[256];
		char *tsdevice = NULL;
		char *calfile = NULL;
		unsigned int i;
	
		int xtemp,ytemp;
		int x,y;
	
		/* Clear the LCD */ 
		LCD_Clear(Blue);
		get_sample (&cal, 0, 50,        50,        "Top left");
	
		get_sample (&cal, 1, XRES - 50, 50,        "Top right");
	
		get_sample (&cal, 2, XRES - 50, YRES - 50, "Bot right");
	
		get_sample (&cal, 3, 50,        YRES - 50, "Bot left");
	
		get_sample (&cal, 4, XRES / 2,  YRES / 2,  "Center");
	
		if (Tp_Calibration (&cal)) {
			printf ("Calibration constants: ");
			for (i = 0; i < 7; i++) 
				printf("%d ", cal.a [i]);
			printf("\n\r");
			sprintf (cal_buffer,"%d %d %d %d %d %d %d",
					cal.a[0], cal.a[1], cal.a[2],
					cal.a[3], cal.a[4], cal.a[5], cal.a[6]);
			printf ("%d %d %d %d %d %d %d",
					cal.a[1], cal.a[2], cal.a[0],
					cal.a[4], cal.a[5], cal.a[3], cal.a[6]);
			i = 0;
		} 
		else {
			printf("Calibration failed.\n\r");
			i = -1;
		}
		while(1)
		{
			while(1){		
				if (pen_flag){
	
					TpGetXY(&point);
					LCD_Clear(White);
					pen_flag = 0;
					break;
				}
			}
			cal.x[0] = point.x;
			cal.y[0] = point.y;
			printf("before Calibration x = %d y=%d\n\r",cal.x[0],cal.y[0]);
			xtemp = cal.x[0];
			ytemp = cal.y[0];
			x = 	(int)(( cal.a[0] +
						cal.a[1]*xtemp + 
						cal.a[2]*ytemp ) / cal.a[6]);
			y =		(int)(( cal.a[3] +
						cal.a[4]*xtemp +
						cal.a[5]*ytemp ) / cal.a[6]);
			printf("after Calibration x = %d y=%d\n\r",x,y);
			LCD_DrawCircle(x, y, 5, Red);
		}
		return i;
	}


## 软件下载、测试验证 ##


