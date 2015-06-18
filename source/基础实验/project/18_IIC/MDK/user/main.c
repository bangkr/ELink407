#include "stm32f4xx.h"
#include "uart.h"
#include "24C64.h"
#include "stdio.h"
#include "string.h"


int main()
{
	uint16_t Addr;
	uint8_t WriteBuffer[256]={0};
	uint8_t ReadBuffer[256]={0};
	init_uart();
	printf("\r\n****************************************************************\r\n");
	
	I2C_Configuration();
	
	for(Addr=0; Addr<256; Addr++){
		WriteBuffer[Addr]=Addr+100;	 /* 填充WriteBuffer */
	}

	//全部擦出，每个字节置0xff
	//I2C_Erase();
	//printf("Erase ok\r\n");
	
	/* 开始向EEPROM写数据 */
	printf("\r\n EEPROM 24C64 Write Test \r\n");
	
	for(Addr=0;Addr<sizeof(WriteBuffer);Addr++){
		printf("%d ",WriteBuffer[Addr]);
	}
	printf("\r\n");
	
	I2C_Write(I2C,ADDR_24L64,0x0,WriteBuffer,sizeof(WriteBuffer) );
	printf("\r\n EEPROM 24C64 Write Test OK \r\n");
	
	/* EEPROM读数据 */
	printf("\r\n EEPROM 24C64 Read Test \r\n");
	I2C_Read(I2C,ADDR_24L64,0x0,ReadBuffer,sizeof(ReadBuffer) );
	
	for(Addr=0;Addr<sizeof(ReadBuffer);Addr++){
		printf("%d ",ReadBuffer[Addr]);
	}
	printf("\r\n");
	
	if(strcmp((const char *)ReadBuffer,(const char *)WriteBuffer)==0){
		printf("\r\n EEPROM 24C64 Read Test ok \r\n");
	}

	while (1)
	{
	}
}
