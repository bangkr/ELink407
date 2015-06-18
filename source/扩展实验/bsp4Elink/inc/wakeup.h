#ifndef __WKUP_H
#define __WKUP_H

#include "stm32f4xx.h"
  
u8 check_wkup(void); //检测PA0脚的信号
void init_wkup(void); //PA0 WKUP唤醒初始化
void StandbyMode(void);//系统进入待机模式

#endif

