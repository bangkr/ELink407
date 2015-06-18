#include "stm32f4xx.h"
#include <stdio.h>
#include "uart.h"
#include "delay.h"
#include "adc.h"

#include "dac.h"

int main()
{
	double adc_value;
	u16 dacval=0; 
	
	init_uart();
	init_delay();
	
	ADC1_CH8_DMA_Config();
	
	dac1_Init();          //DAC通道1初始化 
	DAC_SetChannel1Data(DAC_Align_12b_R,dacval);//初始值为0 
	
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConv(ADC1);
	while (1)
	{		
		adc_value=ADC_filter()*1.0/1000;
		printf("get ADC input value=%.1f V\r\n",adc_value);
		
		if(dacval >= 4000){
			dacval = 0;
		}
		else{
			dacval += 200;
		}
		/*电压值
			Val/3.3 = dacval/4096
			Val = dacval*3.3/4096;
		*/
		DAC_SetChannel1Data(DAC_Align_12b_R, dacval);//设置DAC值
		printf("set DAC output val=%.1f;  ",dacval*3.3/4096);
		delay_ms(500);
	}
}


