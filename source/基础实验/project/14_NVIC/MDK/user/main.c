#include "led.h"
#include "button.h"
#include "button_noexti.h"
#include "stm32f4xx.h"
#include "uart.h"
#include "delay.h"

typedef  unsigned char uint8_t;
typedef  unsigned int uint32_t;
/* 仅允许本文件内调用的函数声明 */

void Delay(uint32_t nCount);

 __IO uint8_t ubPreemptionOccurred;
 __IO uint8_t ubPreemptionPriorityValue = 0;

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	
	init_uart(); 	/* 初始化串口 */
	init_button();
	EXTIX_Init();
	init_delay();
	/* 针对不同的应用程序，添加需要的底层驱动模块初始化函数 */

	init_led(); 		/* 初始LED指示灯端口 */
	

	/* 配置嘀嗒定时器的抢占优先级和子优先级 */
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), !ubPreemptionPriorityValue, 0));
	/* 进入主程序循环体 */
	while (1)
	{
		if(ubPreemptionOccurred != 0)
		{
			/* 翻转 LED1 */
			Led_toggle(1);
			Delay(0x5FFFF);

			/* 翻转 LED2 */
			Led_toggle(2);
			Delay(0x5FFFF);

		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: Delay
*	功能说明: 延迟函数
*	形    参：nCount  延迟计数
*	返 回 值: 无
*********************************************************************************************************
*/
void Delay( uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

