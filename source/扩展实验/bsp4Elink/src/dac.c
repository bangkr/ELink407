#include "dac.h"
#include "stm32f4xx.h"
#include <stdio.h>

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
//设置通道1输出电压 
//vol:0~3300,代表0~3.3V 
void Dac1_Set_Vol(u16 vol) 
{ 
	double temp=vol; 
	temp/=1000; 
	temp=temp*4096/3.3; 
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12位右对齐数据格式
} 
	
