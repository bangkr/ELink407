
#ifndef __TP_H
#define __TP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

typedef struct 
{
  int16_t x;
  int16_t y;
  int16_t z1;
  int16_t z2;
} tp_point_t;   


					  


void TpInit(void);
void TpGetXY(tp_point_t *tpoint);
extern volatile unsigned int pen_flag;

#endif  // __TP_H

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

