#ifndef __CS4334_H
#define __CS4334_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

#define DMA_MAX_SZE          0xFFFF
#define I2S_ENABLE_MASK      0x0400
#define DMA_MAX(x)           (((x) <= DMA_MAX_SZE)? (x):DMA_MAX_SZE)

void AUDIO_Init(u32 AudioFreq);
void I2S_GPIO_Init(void);
void Audio_I2S_Init(u32 AudioFreq);
void Audio_DMA_Init(void);
uint32_t AUDIO_Play(u16* pBuffer, u32 Size);
void Audio_MAL_Play(u32 Addr, u32 Size);

#endif 
