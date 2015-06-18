#include "stm32f4xx.h"
#include "delay.h"
#include "font.h"
#include "lcd.h"

#include "logo.h"

int main(void)
{

	DrawMode Mode;

	Mode.FColor = 0xF800;
	Mode.BColor = 0x001F;
	Mode.Mode = 0;

	init_delay();
	
	init_lcd();	

	while(1)
	{
		LCD_Clear(Red);
		delay_ms(1000);

		LCD_Clear(White);
		delay_ms(1000);

		PutPixel(120, 200, Black);	
		delay_ms(1000);

		LCD_DrawString(0, 0, (uint8_t*)"ÄãºÃ Elink407", &Mode);
		delay_ms(1000);

		LCD_DisplayNum(0, 80, 100, &Mode);
		delay_ms(1000);
		
		LCD_Draw_Imag(60,0,126,400, gImage_logo);
		delay_ms(1000);
		
	}
}

