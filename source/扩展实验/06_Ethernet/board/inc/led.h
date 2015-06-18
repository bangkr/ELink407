#ifndef __LED_H
#define __LED_H

#include "stm32f4xx_conf.h"

typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3
} Led_Def;

static GPIO_TypeDef* LED_PORT[4]={GPIOC, GPIOC, GPIOC, GPIOA};
static const u16 LED_PIN[4]={GPIO_Pin_13, GPIO_Pin_2, GPIO_Pin_3,GPIO_Pin_3};

void LED_Init(void);
void LEDOn(Led_Def Led);
void LEDOff(Led_Def Led);
void LEDTog(Led_Def Led);

#endif 

