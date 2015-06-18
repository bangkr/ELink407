#include "stm32f4xx.h"
#include "uart.h"
#include <stdio.h>
#include "sst25vf.h"

u8 Tx_Buffer[] = "abbbbbgdsgsh";
u8 Rx_Buffer[20];

u8 DataByte=0;

int main()
{
	u32 jedec_id;
	init_uart();
	init_spi();  
	SST25V_Init();  
	SST25V_ChipErase();
	
	jedec_id=SST25V_ReadJedecID();
	printf("JEDEC_ID=%x\r\n",jedec_id);
	
	//test flash 1,²Á³ö¶Á²âÊÔ
	DataByte = SST25V_ByteRead(0x000000);    
	printf("data1=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x000010);  
	printf("data2=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x000100);  
	printf("data4=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x001000); 
	printf("data5=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x010000); 
	printf("data6=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x100000); 
	printf("data7=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x600000); 
	printf("data8=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xF00000); 
	printf("data9=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xFFFFFF); 
	printf("data10=%x\r\n",DataByte);
	
	//test flash 2£¬ÏòµØÖ·0xfff000Ð´Èë0x1000¸ö×Ö·û0x72
	SST25V_AAI_WriteBytes(0x72, 0xFFF000,0x001000);
	
	DataByte = SST25V_ByteRead(0x000000); 
	printf("data11=%x\r\n",DataByte);	
	DataByte = SST25V_ByteRead(0x000001); 
	printf("data12=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0x000002); 
	printf("data12=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xFFF000); 
	printf("data13=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xFFF0EE); 
	printf("data14=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xFFFEEE); 
	printf("data15=%x\r\n",DataByte);
	DataByte = SST25V_ByteRead(0xFFFFFF); 
	
	//test flash 3£¬ÏòµØÖ·0xfff000Ð´×Ö·û´®
	SST25V_ChipErase();
	SST25V_AAI_BufferProgram(Tx_Buffer,0xFFF000,BufferSize);
	printf("tx_buffer=%s\r\n",Tx_Buffer);
	SST25V_BufferRead(Rx_Buffer,0xFFF000,BufferSize); 
	printf("rx_buffer=%s\r\n",Rx_Buffer);
	
	DataByte = SST25V_ByteRead(0xFFEEEE); 
	printf("data16=%d\r\n",DataByte);
	return 0;

}
