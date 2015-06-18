#ifndef __ADC_H_
#define __ADC_H_

#include "stm32f4xx.h"


#define ADC1_DR_ADDRESS    	((uint32_t)0x4001204C)


void ADC1_CH8_DMA_Config(void);
uint32_t ADC_filter(void);

#endif

