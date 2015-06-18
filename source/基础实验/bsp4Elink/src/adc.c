#include "adc.h"
#include "delay.h"

__IO uint16_t ADC1ConvertedValue = 0;

/*滤波算法，ADC采样10次，去掉最高和最低，再取剩下8次平均值*/
uint32_t ADC_filter(void)
{
	uint32_t i,min,max,sum;
	uint32_t arr[10]={0};
	sum=0;
	for(i=0;i<10;i++){
		arr[i]=ADC1ConvertedValue *3300/0xFFF;
		delay_ms(100);
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

/*ADC1channel_8 with DMA configuration*/
void ADC1_CH8_DMA_Config(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;

	/* Enable ADC1, DMA2 and GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA2 Stream0 channel0 configuration **************************************/
	/* 选择DMA频道*/
	DMA_InitStructure.DMA_Channel = DMA_Channel_0; 
	/*DMA外设地址，即ADC数据寄存器地址*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	/*自定义代发数据首地址*/
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
	/*数据传输方向，从外设到内存*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	/*传输数据缓冲区的大小，单位等于外设大小，如外设数据大小为halfword，对应就是16位*/
	DMA_InitStructure.DMA_BufferSize = 1;
	/*禁止外设寄存器自动增加*/
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/*禁止内存地址自增*/
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	/*外设数据的大小*/
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	/*内存数据的大小（等于外设数据缓冲区大小）*/
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	/*传输模式，循环*/
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	/*优先级high*/
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	/*禁止FIFO（先进先出）*/
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	/*FIFO阀值*/
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	/*配置内存和外设传输的方式：单一的传输方式*/
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	/*初始化DMA2_stream0*/
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	/*使能DMA2_stream*/
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* Configure ADC1 Channel8 pin（PB0） as analog input ******************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ADC Common Init*/
	//独立模式
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	//设置ADC时钟频率
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	//禁止多ADC使用DMA模式
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	//两个采样之间的延迟
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_10Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	//分辨率
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//禁止扫描模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	//使能连续转换工作模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	//设置转换触发方式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	//设置数据对其方式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//转换通道
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel_8 configuration *************************************/
	//规则通道设置，选择channel8，规则采样顺序1-16，转换时间28个周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_28Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}
