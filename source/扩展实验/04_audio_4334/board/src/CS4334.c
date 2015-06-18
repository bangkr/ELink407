#include "CS4334.h"

DMA_InitTypeDef DMA_InitStructure;
u32 AudioTotalSize=0xFFFF; 
u32 AudioRemSize  =0; 
u16 *CurrentPos;           

extern void AUDIO_TransferComplete(u32 pBuffer, uint32_t Size);
void AUDIO_Init(u32 AudioFreq)
{
	I2S_GPIO_Init();
	Audio_I2S_Init(AudioFreq);
	Audio_DMA_Init();
}

void I2S_GPIO_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_15; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_SPI2);  
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);   
  	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_SPI2); 
}

void Audio_I2S_Init(uint32_t AudioFreq)
{
  	I2S_InitTypeDef I2S_InitStructure;
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  	SPI_I2S_DeInit(SPI2);
  	I2S_InitStructure.I2S_AudioFreq = AudioFreq;
  	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable; 
  	I2S_Init(SPI2, &I2S_InitStructure);
}

//DMA传送配置
void Audio_DMA_Init(void)  
{ 
  	NVIC_InitTypeDef NVIC_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  	DMA_Cmd(DMA1_Stream4, DISABLE);
  	DMA_DeInit(DMA1_Stream4);
  	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  	DMA_InitStructure.DMA_PeripheralBaseAddr = 0x4000380C;
  	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;//该项将在播放函数中进行配置
  	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  	DMA_InitStructure.DMA_BufferSize = (uint32_t)0xFFFE;//该项将在播放函数中进行配置      
  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
  	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
  	DMA_Init(DMA1_Stream4, &DMA_InitStructure);  
  	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);//使能所选DMA的中断
  	//I2S DMA中断通道配置
  	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级(0 最高)
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级(0 最高)
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);   
  	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);//中断请求使能  
}


//开始从特定大小（Size）的缓冲区（pBuffer）中播放音频流
uint32_t AUDIO_Play(u16* pBuffer, u32 Size)
{
  	AudioTotalSize=Size; 
  	Audio_MAL_Play((u32)pBuffer,(u32)(DMA_MAX(Size/4)));//更新媒体层数据并播放
  	AudioRemSize=(Size/2)-DMA_MAX(AudioTotalSize);//更新剩余数据量
  	CurrentPos=pBuffer+DMA_MAX(AudioTotalSize);//更新当新数据指针 
  	return 0;
}

//开始播放媒体中的音频流
void Audio_MAL_Play(u32 Addr, u32 Size)
{         
  	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)Addr;//配置缓冲区地址和大小
  	DMA_InitStructure.DMA_BufferSize=(uint32_t)Size/2;
  	DMA_Init(DMA1_Stream4,&DMA_InitStructure);
  	DMA_Cmd(DMA1_Stream4,ENABLE);//I2S DMA使能   
  	if ((SPI2->I2SCFGR & I2S_ENABLE_MASK)==0)I2S_Cmd(SPI2,ENABLE);//若I2S接口未使能，则使能
}

//I2S中断处理
void SPI2_IRQHandler(void)
{
  	if (SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)!=RESET)
  	{     	
    	SPI_I2S_SendData(SPI2, 0);//发送空字节到I2S 
  	}
}

//DMA中断处理函数
void DMA1_Stream4_IRQHandler(void)
{    
  	//传送完成中断
  	if (DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4)!=RESET)
  	{         
    	if (AudioRemSize>0)//查看是否到达文件尾
    	{   
      		while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);//等待DMA disabled     
      		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);//清传送完成中断标志           
      		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) CurrentPos;//重新配置缓冲区地址和大小
      		DMA_InitStructure.DMA_BufferSize = (uint32_t) (DMA_MAX(AudioRemSize));            
      		DMA_Init(DMA1_Stream4, &DMA_InitStructure);
      		DMA_Cmd(DMA1_Stream4, ENABLE);    
      		CurrentPos += DMA_MAX(AudioRemSize);//更新数据指针        
      		AudioRemSize -= DMA_MAX(AudioRemSize);//更新剩余数据   
      		DMA_Cmd(DMA1_Stream4, ENABLE); 
    	}
    	else//已到达文件尾
    	{
      		DMA_Cmd(DMA1_Stream4, DISABLE);   
      		DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);       
      		AUDIO_TransferComplete((uint32_t)CurrentPos, 0);       
    	}
  	}
}









