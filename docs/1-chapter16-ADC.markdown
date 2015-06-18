# ADC实验 #

ADC即（Analog-to-Digital Converter）的缩写，模/数转换器或者模拟/数字转换器，意为将连续变量的模拟信号转换为离散的数字信号的器件。

## 实验原理与基础知识 ##
stm32f407系列ADC支持DMA(Direct Memory Access，直接内存存取)模式，利用DMA对数据进行传输，从而提高ADC效率，在ADC每次转换完之后，由DMA控制器把转换值从ADC数据寄存器中转移到内存，即把数据从外设直接写入内存，把得到数据加以滤波算法分析，最终将得到结果通过串口打印显示。

对于stm32f407，共有两个DMA控制器，总共有16个stream（流），每个DMA控制器包括8个stream，每个stream多达8个channel（通道），本案例选择DMA2stream0->Channel_0。

stm32f407的每个ADC有19个复用channel（通道），可以检测16个外部信号源和两个内部信号源和电池信号。可以对应电路原理图和stm32f07数据手册进行选择，这里选择使用ADC1的8通道。

## 电路设计 ##
本案例使用的ADC硬件为电位器，通过旋转电位器的螺丝得到不同的模拟信号，电位器连接到CPU的PB0，该引脚支持ADC1和ADC2的channel_8，注意电位器与温湿度传感器复用ADC引脚，这里需将J27到1、2用跳线冒连接。

![](img/chapter0/adc_sch.png) 

## 库函数和数据结构 ##

ADC基本配置直接相关的几个固件库函数。这些函数和定义主要分布在 stm32f4xx_adc.h 和 stm32f4xx_adc.c 文件中。

使能ADC时钟总线：

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        
ADC初始化相关数据结构：

        ADC_InitTypeDef       ADC_InitStructure;
		ADC_CommonInitTypeDef ADC_CommonInitStructure;
        
初始化ADC外设接口：

        ADC_CommonInit(&ADC_CommonInitStructure);
        
初始化ADC配置：

        ADC_Init(ADC1, &ADC_InitStructure);
        
规则通道设置，1个channel，转换时间为28个周期

        ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles);
        
启用ADC1的DMA请求后开始数据转移：

        ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
        
启用ADC1到DMA请求：

        ADC_DMACmd(ADC1, ENABLE);
        
启用ADC1外设：

        ADC_Cmd(ADC1, ENABLE);

## 软件设计、开发 ##

在工程中添加adc.c 文件和adc.h 文件，ADC相关的函数和定义都在这两个文件中。

1)  使能ADC、DMA和GPIO到时钟总线。

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOB, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        
2)  定义数据结构变量

        ADC_InitTypeDef       ADC_InitStructure;
        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        DMA_InitTypeDef       DMA_InitStructure;
        GPIO_InitTypeDef      GPIO_InitStructure;

3)  配置DMA，选择DMA2的stream0

        DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
        DMA_InitStructure.DMA_BufferSize = 1;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;
        DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
        DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
        DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
        DMA_Init(DMA2_Stream0, &DMA_InitStructure);
        DMA_Cmd(DMA2_Stream0, ENABLE);
        
4)  配置ADC对应GPIO为Analog Mode

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        
5)  初始化ADC1

    	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;
        ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
        ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
        ADC_InitStructure.ADC_NbrOfConversion = 1;
        ADC_Init(ADC1, &ADC_InitStructure);
    
6)  开启ADC1

        /* ADC1 regular channel_8 configuration *************************************/
        ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles);
        /* Enable DMA request after last transfer (Single-ADC mode) */
        ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
        /* Enable ADC1 DMA */
        ADC_DMACmd(ADC1, ENABLE);
        /* Enable ADC1 */
        ADC_Cmd(ADC1, ENABLE);

7)  编写滤波函数，让ADC采样十次，去掉最高和最低，再取剩下8次平均值

        uint32_t ADC_filter(void)
        {
            uint32_t i,min,max,sum;
            uint32_t arr[10]={0};
            sum=0;
            for(i=0;i<10;i++){
                arr[i]=ADC1ConvertedValue *3300/0xFFF;
                delay_ms(10);
                sum += arr[i];
            }

            min=arr[0];
            max=arr[0];
            for(i=0;i<10;i++){
                if(arr[i]<min)
                    min=arr[i];
                if(arr[i]>max)
                    max=arr[i];
            }

            return (sum-min-max)>>3;
        }
8）串口配置（略），请参考实验

9) 编写main函数

        #define ADC1_DR_ADDRESS    	((uint32_t)0x4001204C)
        __IO uint16_t ADC1ConvertedValue = 0;
        
        int main(void)
        {
            float adc_value;

            /*uart1 configure*/
            uart_config();

            /* ADC1 configuration */	
            ADC1_CH8_DMA_Config();

            /* Start ADC1 Software Conversion */ 
            ADC_SoftwareStartConv(ADC1);

            while (1)
            {		
                adc_value=ADC_filter()*1.0/1000;
                printf("value=%f\r\n",adc_value);
            }

        }

## 软件下载、测试验证 ##


