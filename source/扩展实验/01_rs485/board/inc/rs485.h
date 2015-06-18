#ifndef __RS485_H
#define __RS485_H			 
 								  
	  	
extern unsigned char RS485_RX_BUF[64]; 		//接收缓冲,最大64个字节
extern unsigned char RS485_RX_CNT;   			//接收到的数据长度

//模式控制
#define RS485_TX_EN		GPIO_ResetBits(GPIOB,GPIO_Pin_8)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，设置EN_USART2_RX为1，否则设置为0
#define EN_USART3_RX 	1			//0,不接收;1,接收.

														 
void RS485_Init(unsigned int pclk2,unsigned int  bound);
void RS485_Send_Data(unsigned char *buf,unsigned char len);
void RS485_Receive_Data(unsigned char *buf,unsigned char *len);		 
#endif	   
















