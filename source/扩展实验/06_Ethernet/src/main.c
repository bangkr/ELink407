#include "main.h"

#define SYSTEMTICK_PERIOD_MS  10
uint32_t timingdelay;
__IO u32 LocalTime=0; 

 


int main(void)
{

	init_uart();
	
	 
	
	ETH_BSP_Config();
	
 

	LwIP_Init();
	httpd_init();
  while (1)
  {  
    	if(ETH_CheckFrameReceived())//检测是否收到数据包
    	{ 
      		LwIP_Pkt_Handle();//处理数据包
    	}
    	LwIP_Periodic_Handle(LocalTime);
  }  
}

void Delay(uint32_t nCount)
{
  	timingdelay=LocalTime+nCount;  
  	while(timingdelay>LocalTime);
}

void Time_Update(void)
{
  	LocalTime+=SYSTEMTICK_PERIOD_MS;
}


