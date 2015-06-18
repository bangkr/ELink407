#include "stdio.h"
#include "delay.h"  
#include "uart.h"   
#include "led.h"
#include "button_noexti.h"    
#include "malloc.h" 

#include "sdio_sdcard.h"    

//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}

int main(void)
{        
	u8 key;		 
	u32 sd_size;
	u8 t=0,ledblink=0;	
	u8 *buf;

	
	init_delay();			//延时初始化  
	init_uart();		//初始化串口波特率为115200 
	init_led();					//初始化LED  
 	init_button();					//按键初始化  
	
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	
 	 
	printf("Card test\r\n");
	 
 	while(SD_Init())//检测不到SD卡
	{
		printf("SD Card Error\r\n");
		
		delay_ms(500);					
		
		ledblink^=1;
		if(ledblink)
			led_on(LED1);//提示系统正在运行
		else
			led_off(LED1);
	}
	show_sdcard_info();	//打印SD卡相关信息
 	 
	//检测SD卡成功
	printf("SD Card Size:%d\r\n",(int)SDCardInfo.CardCapacity);	
	
	while(1)
	{
		key=button_read();
		if((1<<0)==key)//KEY0按下,发送一次数据
		{
			buf=mymalloc(0,512);		//申请内存
			if(SD_ReadDisk(buf,0,1)==0)	//读取0扇区的内容
			{	
				printf("读取0扇区的内容\r\n");
				
				printf("SECTOR 0 DATA:\r\n");
				for(sd_size=0;sd_size<512;sd_size++)printf("%x ",buf[sd_size]);//打印0扇区数据    	   
				printf("\r\nDATA ENDED\r\n");
				
			}
			myfree(0,buf);//释放内存	   
		}   
		t++;
		delay_ms(10);
		if(t==20)
		{
			ledblink^=1;
			if(ledblink)
				led_on(LED1);//提示系统正在运行
			else
				led_off(LED1);
			t=0;
		}
	} 
}
















