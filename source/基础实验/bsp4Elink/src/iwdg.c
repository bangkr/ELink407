#include "iwdg.h"
#include "stdio.h"
#include "stm32f4xx_iwdg.h"

//初始化独立看门狗
//prer:分频数:0~7(只有低 3 位有效!)    rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是 256!
//rlr:重装载寄存器值:低 11 位有效. 

//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).

void init_iwdg(unsigned char prer,short rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	//取消寄存器写保护
	IWDG_SetPrescaler(prer); 						//设置 IWDG 分频系数
	IWDG_SetReload(rlr);    						//设置 IWDG 装载值
	IWDG_ReloadCounter(); 							//reload
	IWDG_Enable();        							//使能看门狗
}

//喂独立看门狗
void feed_iwdg(void)
{
	printf("喂狗\r\n");
	IWDG_ReloadCounter();//reload
}
