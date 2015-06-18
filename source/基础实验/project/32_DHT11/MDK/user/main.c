#include "stm32f4xx.h"
#include "delay.h"
#include "stdio.h"
#include "uart.h"
#include "lcd.h"


#include "dht11.h"

int main()
{
	u8 temperature; 
	
	u8 humidity;
	
	DrawMode Mode;
	
	Mode.FColor = 0xF800;
	Mode.BColor = 0x001F;
	Mode.Mode = 0;
	
	init_delay();
	
	init_uart();
	
	init_DHT11();
	
	init_lcd();
	
	while(1)
	{
		LCD_Clear(White);
		
		DHT11_Read_Data(&temperature,&humidity);		//¶ÁÈ¡ÎÂÊª¶ÈÖµ
		
		printf("DHT11  temperature: %d  humidity: %d\r\n",temperature,humidity);	
		
		LCD_DisplayNum(0, 80, temperature, &Mode);
		
		LCD_DisplayNum(0, 100, humidity, &Mode);
		
		delay_ms(1000);
	}
				
		
}


