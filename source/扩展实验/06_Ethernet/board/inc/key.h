#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx_conf.h"

typedef enum 
{  
  KEY1 = 0,
  KEY2 = 1,
  KEY3 = 2,
  KEY4 = 3,
} Key_Def;

static const u16 KEY_PIN[4]={GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_10};
static const u16 KEY_SRC[4]={EXTI_Line0, EXTI_Line1, EXTI_Line2, EXTI_Line10};
static const u8  KEY_PNS[4]={EXTI_PinSource0, EXTI_PinSource1, EXTI_PinSource2, EXTI_PinSource10};
static const u8  KEY_IRQ[4]={EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI15_10_IRQn};

void Key_Init(void);
u8 Get_Key(Key_Def key);
u8 KEY_Scan(void);
void Set_Keyint(Key_Def key);

#endif

