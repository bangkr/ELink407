#include "stm32f4xx.h"
#include "led.h"
#include "button_noexti.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"

#define BUFFER_SIZE  84

static const unsigned int testData[]={
0x3f504e47,0x0d0a1a0a,0x0000000d,0x49484452,  
0x0000013f,0x0000003f,0x08060000,0x003f7e3f,  
0x25000000,0x06624b47,0x44003f00,0x3f003f3f,  
0x3f3f3f00,0x00000970,0x48597300,0x000e3f00,  
0x000e3f01,0x3f2b0e1b,0x00002000,0x49444154,  
0x783f3f3f,0x773f5d45,0x3f3f3f33,0x3f3fdbb6,  
0x65135209,0x3f574504,0x15293f3f,0x3f0a3f08,  
0x3f3f3f3f,0x130b283f,0x02023f17,0x0b3f5714,  
0x3f223f0d,0x14010b45,0x1145404a,0x407a3f10,  
0x3f3f265b,0x3f3f727a,0x3f3f3f71,0x3f244b4c,  
0xd9843f7b,0x773f3f3f,0x755f3f3f,0x3f733f39,  
0x3b3f3f3f,0x3c3f3c3f,0x0c3f3f43,0x20103f3f,  
0x743f5c48,0x3f3e403f,0x3f3f143f,0x403f6968,  
0x3f0d1008,0x04003f3f,0x25587d3f,0x3f00483f,  
0x4d110804,0x3f463f3f,0x40302e20,0x0c71023f,  
0x3f3f053f,0x3f442010,0x6c1a213f,0x023f7821,  
0x7f3fdfb0,0x3f3f6f3f,0x3fd8bb5a,0x6d3f4020,  
0x3f384234,0x053f3f3f,0x3f3f3300,0x25783f3f,  
0x5f3f5f3f,0x4b3f3f11,0x08043f3f,0x104d3f60,  
0x3f3f763f,0x20651c3f,0x57c0b33f,0x3f09443f,  
0x563f2410,0x085e3f10,0x4d3f603f,0x3f003f43
 };


int main()
{
	uint8_t testValue = 0x11; 
	
	init_delay();
	
	init_uart();
	
	/* 使能CRC时钟 */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); 
	
	CRC_ResetDR();
	
	/* 计算 "testData" 数据块的CRC */ 
	printf(" testData CRC = %u\r\n", CRC_CalcBlockCRC((uint32_t *)testData,BUFFER_SIZE )); 
	
	/* 计算 单个数据 的CRC */
	while(1)
	{
 
		printf("testVaule = %u    ", testValue); 
	
		CRC_SetIDRegister(testValue++); 

		printf("CRC = %u\r\n", CRC_GetIDRegister());
		
		delay_ms(1000);
	}
}	

