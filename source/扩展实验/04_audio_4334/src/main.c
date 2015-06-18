#include "main.h"

int main(void)
{
	//初始化串口，配波特率置
	init_uart();
	printf("test!\r\n");
	
	//播放sd卡中wave格式音频文件： "/WAV/melody1.wav”
	Wavplay();
  	while(1);
}


