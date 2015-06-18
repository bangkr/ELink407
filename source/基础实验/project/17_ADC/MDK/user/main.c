#include "stm32f4xx.h"
#include "uart.h"
#include "delay.h"
#include <stdio.h>

#include "adc.h"


int main()
{
	
	float adc_value;
	
	init_delay();
	init_uart();
	
	ADC1_CH8_DMA_Config();
	
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConv(ADC1);
	while (1)
	{		
		adc_value=ADC_filter()*1.0/1000;
		printf("value=%f V\r\n",adc_value);
		delay_ms(1000);
	}
}
