#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx_conf.h"


void TIM3_Init(u32 TIM_scale, u32 TIM_Period);
void TIM3_Cinit(u32 TIM_scale ,u16 CCR1 ,u16 CCR2 ,u16 CCR3);

#endif
