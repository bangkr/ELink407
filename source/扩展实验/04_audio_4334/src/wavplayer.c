#include "wavplayer.h"
#include "cs4334.h"
#include "ff.h"
#include <stdio.h>

u16 buffer1[1024]={0x00};
u16 buffer2[1024]={0x00};         
__IO u32 WaveLen=0;
__IO u32 XferCplt=0;
__IO u32 DataOffset=0;
__IO u32 WaveCounter;
u8 buffer_switch=1;
WAVE_TypeDef WAVE_Format;

void Wavplay(void)
{
	FATFS fatfs;            
	FIL fileR;
	UINT BytesRead;	
	u8 i=0;

  	f_mount(0, &fatfs);
 // 	f_open(&fileR, "0:/WAV/melody1.wav" , FA_READ);
//	f_open(&fileR, "0:/MUSIC/1.wav" , FA_READ);
	f_open(&fileR, "0:/wav/1.wav" , FA_READ);
	f_read(&fileR, buffer1, 1024, &BytesRead);
	printf("file read ");
	while(WaveParsing()) printf("file read error");

  	WaveLen = WAVE_Format.DataSize;
  	AUDIO_Init(WAVE_Format.SampleRate);
  	f_lseek(&fileR, WaveCounter);//跳过文件头
  	f_read(&fileR, buffer1, 1024, &BytesRead); 
  	f_read(&fileR, buffer2, 1024, &BytesRead);
  	Audio_MAL_Play((u32)buffer1, 1024);
  	buffer_switch=1;
  	XferCplt=0;  
  	while(WaveLen!=0)
  	{ 
      	while(XferCplt==0);
      	XferCplt=0;
      	if(buffer_switch==0)
      	{
        	Audio_MAL_Play((u32)buffer1,1024);//从buffer1播放
        	f_read(&fileR,buffer2,1024,&BytesRead);//填充buffer2
        	buffer_switch=1;
      	}
      	else 
      	{   
        	Audio_MAL_Play((u32)buffer2,1024);//从buffer2播放
        	f_read(&fileR,buffer1,1024,&BytesRead);//填充buffer1
        	buffer_switch=0;
      	} 
		i++;
		if(i==100)
		{
			i=0;
		}
  	}
}

u8 WaveParsing(void)
{
  	u32 temp=0x00;
  	u32 extraformatbytes=0;

  	temp=ReadUnit((u8*)buffer1,0,4,BigEndian);//读'RIFF'
  	if(temp!=CHUNK_ID)return 1;
  	WAVE_Format.RIFFchunksize=ReadUnit((u8*)buffer1,4,4,LittleEndian);//读文件长度
  	temp=ReadUnit((u8*)buffer1,8,4,BigEndian);//读'WAVE'
  	if(temp!=FILE_FORMAT)return 2;
  	temp=ReadUnit((u8*)buffer1,12,4,BigEndian);//读'fmt '
  	if(temp!=FORMAT_ID)return 3;
  	temp=ReadUnit((u8*)buffer1,16,4,LittleEndian);//读'fmt'数据长度
  	if(temp!=0x10)extraformatbytes=1;
  	WAVE_Format.FormatTag=ReadUnit((u8*)buffer1,20,2,LittleEndian);//读音频格式
  	if(WAVE_Format.FormatTag!=WAVE_FORMAT_PCM)return 4;  
  	WAVE_Format.NumChannels=ReadUnit((u8*)buffer1,22,2,LittleEndian);//读声道数 
	WAVE_Format.SampleRate=ReadUnit((u8*)buffer1,24,4,LittleEndian);//读采样率
	WAVE_Format.ByteRate=ReadUnit((u8*)buffer1,28,4,LittleEndian);//读波特率
	WAVE_Format.BlockAlign=ReadUnit((u8*)buffer1,32,2,LittleEndian);//读块对齐
	WAVE_Format.BitsPerSample=ReadUnit((u8*)buffer1,34,2,LittleEndian);//读采样点位数
	if(WAVE_Format.BitsPerSample!=BITS_PER_SAMPLE_16)return 5;
	DataOffset=36;
	if(extraformatbytes==1)
	{
		temp=ReadUnit((u8*)buffer1,36,2,LittleEndian);//读额外格式字节
		if(temp!=0x00)return 6;
		temp=ReadUnit((u8*)buffer1,38,4,BigEndian);//读'fact'
		if(temp!=FACT_ID)return 7;
		temp=ReadUnit((u8*)buffer1,42,4,LittleEndian);//读Fact数据大小
		DataOffset+=10+temp;
	}
	temp=ReadUnit((u8*)buffer1,DataOffset,4,BigEndian);//读'data'
	DataOffset+=4;
	if(temp!=DATA_ID)return 8;
	WAVE_Format.DataSize=ReadUnit((u8*)buffer1,DataOffset,4,LittleEndian);//读采样数据大小
	DataOffset+=4;
	WaveCounter=DataOffset;
	return 0;
}

u32 ReadUnit(u8 *buffer,u8 idx,u8 NbrOfBytes,Endianness BytesFormat)
{
  	u32 index=0;
  	u32 temp=0;
  
  	for(index=0;index<NbrOfBytes;index++)temp|=buffer[idx+index]<<(index*8);
  	if (BytesFormat == BigEndian)temp= __REV(temp);
  	return temp;
}

void AUDIO_TransferComplete(u32 pBuffer, u32 Size)
{  
  	XferCplt=1;
  	if(WaveLen)WaveLen-=1024;
  	if(WaveLen<1024) WaveLen=0;
}


