#include "stm32f4xx.h"
#include "stdio.h"
#include "delay.h"  
#include "uart.h"   
#include "led.h"
#include "button.h"  
#include "button_noexti.h" 

#include "can.h"

    
int main(void)
{   
	u8 key;
	u8 i=0,t=0,ledblink=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1;//CAN工作模式;0,普通模式;1,环回模式
	
	init_delay();			//延时初始化  
	init_uart();		//初始化串口波特率为115200  	 
	init_led();					//初始化LED 
 	init_button(); 				//按键初始化  
 	
	CAN1_Mode_Init(1,6,7,6,1);	//CAN初始化,波特率500Kbps    

 		
 	while(1)
	{
		key=button_read();
		if((1<<0)==key)//KEY0按下,发送一次数据
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;//填充发送缓冲区
				printf("sentData:0x%x",canbuf[i]);
 			}
			res=CAN1_Send_Msg(canbuf,8);//发送8个字节 
			if(res)printf("Failed\r\n");		//提示发送失败
			else printf("OK\r\n"); 	 		//提示发送成功								   
		}else if((1<<1)==key)//KEY1按下，改变CAN的工作模式
		{	   
			mode=!mode;
			CAN1_Mode_Init(1,6,7,6,mode);	//CAN普通模式初始化,普通模式,波特率500Kbps
  			 
			if(mode==0)//普通模式，需要2个开发板
			{
				printf("Nnormal Mode \r\n");
					    
			}else //回环模式,一个开发板就可以测试了.
			{
				printf("LoopBack Mode\r\n");
 				
			}
 			
		}		 
		key=CAN1_Receive_Msg(canbuf);
		if(key)//接收到有数据
		{			
			
 			for(i=0;i<key;i++)
			{									    
				printf("rev :%d\r\n",canbuf[i]);
			}
				
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			ledblink^=1;
			if(ledblink)
				led_on(LED1);//提示系统正在运行	
			t=0;
			cnt++;
			
		}		   
	} 
}
