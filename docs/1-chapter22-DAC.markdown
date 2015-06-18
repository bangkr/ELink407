# DAC实验 #

STM32F4的DAC(数字/模拟转换）模块是12位数字输入，电压输出型的DAC。

本案例使用DAC输出电压，在利用ADC采样获取DAC到值并用过串口打印转换结果。

![](img/chapter0/dac_result.png) 

## 实验原理与基础知识 ##
STM32F4的DAC可以配置为8位或12位模式，也可以与DMA控制器配合使用。DAC工作在12位模式时，数据可以设置成左对齐或右对齐。DAC模块有2个输出通道，每个通道都有单独的转换器。在双DAC模式下，2个通道可以独立地进行转换，也可以同时进行转换并同步地更新2个通道的输出。 

STM32F4的DAC模块主要特点有： 

①  2个DAC转换器：每个转换器对应1个输出通道   

②  8位或者12位单调输出   

③  12位模式下数据左对齐或者右对齐   

④  同步更新功能   

⑤  噪声波形生成   

⑥  三角波形生成   

⑦  双DAC通道同时或者分别转换 

⑧  每个通道都有DMA功能        

## 电路设计 ##

使用2.0转2.54杜邦线连接J28到Pin5（PA4）和J21的Pin2（PB0）相连接

![](img/chapter0/dac_sch.png) 

## 库函数和数据结构 ##

开启DAC1时钟：

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//使能DAC时钟 

初始化DAC,设置DAC的工作模式：

        DAC_InitTypeDef DAC_InitType; 
        DAC_InitType.DAC_Trigger=DAC_Trigger_None;  //不使用触发功能  TEN1=0 
        DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生 
        DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0; 
        DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;  //DAC1 输出缓存关闭 
        DAC_Init(DAC_Channel_1,&DAC_InitType);    //初始化DAC通道1 

使能DAC转换通道： 

        DAC_Cmd(DAC_Channel_1, ENABLE);    //使能DAC通道
        
设置DAC的输出值。  

        DAC_SetChannel1Data(DAC_Align_12b_R, 0);    //12位右对齐数据格式设置DAC值，初始值0 

读取DAC通道转换的数值： 

        DAC_GetDataOutputValue(DAC_Channel_1);

## 软件设计、开发 ##

在ADC案例中直接添加dac相关代码

1）初始化DAC1

        //DAC通道1输出初始化 
        void dac1_Init(void) 
        {    
            GPIO_InitTypeDef    GPIO_InitStructure; 
            DAC_InitTypeDef DAC_InitType; 

            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//①使能PA时钟 
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//②使能DAC时钟 

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入 
            GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉 
            GPIO_Init(GPIOA, &GPIO_InitStructure);//①初始化GPIO 
            DAC_InitType.DAC_Trigger=DAC_Trigger_None; //不使用触发功能  TEN1=0 
            DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//不使用波形发生 
            DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0; 

            //屏蔽、幅值设置 
            DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;//输出缓存关闭   
            DAC_Init(DAC_Channel_1,&DAC_InitType);   //③初始化DAC通道1 
            DAC_Cmd(DAC_Channel_1, ENABLE);    //④使能DAC通道1 
            DAC_SetChannel1Data(DAC_Align_12b_R, 0);    //⑤12位右对齐数据格式 
        } 
        
2）编写住主函数，让DAC输出电压逐渐增大，并通过ADC获取打印结果

        int main()
        {
            double adc_value;
            u16 dacval=0; 
            uart_config();
            ADC1_CH8_DMA_Config();

            dac1_Init();          //DAC通道1初始化 
            DAC_SetChannel1Data(DAC_Align_12b_R,dacval);//初始值为0 

            /* Start ADC1 Software Conversion */ 
            ADC_SoftwareStartConv(ADC1);
            while (1)
            {		
                adc_value=ADC_filter()*1.0/1000;
                printf("get ADC input value=%.1f V\r\n",adc_value);

                if(dacval >= 4000){
                    dacval = 0;
                }
                else{
                    dacval += 200;
                }
                /*电压值
                    Val/3.3 = dacval/4096
                    Val = dacval*3.3/4096;
                */
                DAC_SetChannel1Data(DAC_Align_12b_R, dacval);//设置DAC值
                printf("set DAC output val=%.1f;  ",dacval*3.3/4096);
                delay_ms(500);
            }
        }
    
## 软件下载、测试验证 ##

