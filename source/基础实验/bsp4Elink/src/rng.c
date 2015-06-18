#include "rng.h"
#include "delay.h"
#include "stm32f4xx.h"

int init_rng(void)
{
	u16 retry=0; 
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);  //开启 RNG 时钟   
	RNG_Cmd(ENABLE);  //使能 RNG
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)//等待就绪
	{  
		retry++; 
		delay_ms(1); 
	}
	if(retry>=10000)return 1;//随机数产生器工作不正常
	return 0;
}

//得到随机数
//返回值:获取到的随机数
unsigned int get_RandomNum(void)
{ 
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);  //等待随机数就绪
	return RNG_GetRandomNumber(); 
}

//生成[min,max]范围的随机数
unsigned int get_RandomRange(int min,int max)
{ 
	return get_RandomNum()%(max-min+1) +min;
}

