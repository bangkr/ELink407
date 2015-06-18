#ifndef __DHT11_H
#define __DHT11_H 
#include "delay.h" 
#include "stm32f4xx.h"


//IO输入输出设置

#define DHT11_IO_IN()  {GPIOB->MODER&=~(3<<(0*2));GPIOB->MODER|=0<<0*2;}	//PB0输入模式
#define DHT11_IO_OUT() {GPIOB->MODER&=~(3<<(0*2));GPIOB->MODER|=1<<0*2;} 	//PB0输出模式 

u8 init_DHT11(void);	//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);	//检测是否存在DHT11
void DHT11_Rst(void);	//复位DHT11    
#endif















