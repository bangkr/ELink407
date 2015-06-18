#include "delay.h"  
#include "uart.h"   
#include "led.h"
#include "button.h"  
#include "button_noexti.h"  
#include "rs485.h"
#include "stm32f4xx.h"


int main(void)
{   
	u8 key;
	u8 i=0,t=0,ledblink=0;
	u8 cnt=0;
	u8 rs485buf[5]; 
	
	init_delay();			//延时初始化  
	init_uart();		//初始化串口波特率为115200  	 
	init_led();					//初始化LED 
	init_button(); 				//按键初始化  

	RS485_Init(42,9600);		//初始化RS485
	 									  
	while(1)
	{
		key=button_read();
		if((1<<0)==key)//KEY0按下,发送一次数据
		{
			for(i=0;i<5;i++)
			{
				rs485buf[i]=cnt+i;//填充发送缓冲区
 			}
			RS485_Send_Data(rs485buf,5);//发送5个字节 									   
		}	
		
		RS485_Receive_Data(rs485buf,&key);
		if(key)//接收到有数据
		{
			if(key>5)
				key=5;//最大是5个数据.
 			for(i=0;i<key;i++)
				;
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
			cnt++;
		}		   
	}   
}

















