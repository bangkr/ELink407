#include "stm32f4xx.h"
#include "delay.h"  
#include "uart.h"   
#include "led.h"
#include "button_noexti.h"  
 
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	 
#include "wm8978.h"	 
#include "audioplay.h"	


 
int main(void)
{        
 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	init_delay();  //初始化延时函数
	init_uart();		//初始化串口波特率为115200
	init_led();					//初始化LED 
	//usmart_dev.init(84);		//初始化USMART
 	
 	init_button();					//按键初始化  
	
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(40,40);	//耳机音量设置
	WM8978_SPKvol_Set(50);		//喇叭音量设置
	
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[0],"0:",1); 		//挂载SD卡  
	   
    delay_ms(1000);	

	while(1)
	{ 
		audio_play();
	} 
}







