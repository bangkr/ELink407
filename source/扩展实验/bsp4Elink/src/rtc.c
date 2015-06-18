#include "rtc.h"
#include <stdio.h>


/* RTC Configuration ******************************************************/
void rtc_config(void)
{	
	/* RTC_ReadBackupRegister(RTC_BKP_DR0) */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);//打开PWR时钟
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);//打开配置寄存器权限
	
	/* Clear Wakeup flag */
	PWR_ClearFlag(PWR_FLAG_WU);

    /* Reset Backup Domain */
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
	
	
    /* Enable the LSE */ 
    RCC_LSEConfig(RCC_LSE_ON);//使用外部晶振
  	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET){}//等待外部晶振准备好
	
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择RTC时钟源  
	
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
    RTC_WaitForSynchro();//等待RTC APB寄存器同步
		
}


void rtc_time_init(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	RTC_InitTypeDef RTC_InitStructure;
	struct RtcTime time = {0x14,0x09,0x10,0x03,0x0,0x0,0x0};
	
	//if the time not set(first boot)
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2){
		//printf("init...\r\n");
		rtc_config();
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
		RTC_TimeStructure.RTC_Hours = time.hour;
		RTC_TimeStructure.RTC_Minutes = time.min;
		RTC_TimeStructure.RTC_Seconds = time.sec;
		RTC_DateStructure.RTC_Year = time.year;
		RTC_DateStructure.RTC_Month = time.month;
		RTC_DateStructure.RTC_Date = time.date;
		RTC_DateStructure.RTC_WeekDay = time.week;
	
		// prescaler factor
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv = 0XFF;
		
		// Specifies the RTC Hour Format
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	
		RTC_Init(&RTC_InitStructure);
		RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
		RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
		
		//Writes a data in a specified RTC Backup data register
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
	}//the time already set(reboot)
	else{   
		//printf("reset..\r\n");
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	    PWR_BackupAccessCmd(ENABLE);
	    RTC_WaitForSynchro();
  	}
	
}

void get_rtc_time(void)
{
	struct RtcTime CurrentTime;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure); //读日期
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure); //读时间
	
	CurrentTime.hour = RTC_TimeStructure.RTC_Hours;
	CurrentTime.min = RTC_TimeStructure.RTC_Minutes;
	CurrentTime.sec = RTC_TimeStructure.RTC_Seconds;
	CurrentTime.year = RTC_DateStructure.RTC_Year;
	CurrentTime.month = RTC_DateStructure.RTC_Month;
	CurrentTime.date = RTC_DateStructure.RTC_Date;
	CurrentTime.week = RTC_DateStructure.RTC_WeekDay;
	
	printf("20%02x-%02x-%02x %02x:%02x:%02x\r\n",CurrentTime.year,
		CurrentTime.month,CurrentTime.date,CurrentTime.hour,CurrentTime.min,CurrentTime.sec);
}


void rtc_alarm_config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;

	/* 初始化外部中断17（参考数据手册） */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line=EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* 初始化RTC_Alarm_IRQn中断 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* RTC闹钟初始化， 每到5秒闹钟触发 */
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE); 
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x10;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	
	/* 可选择屏蔽日期，星期和时间，如果全部屏蔽，每隔一秒产生一次闹钟 */
	//RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_AlarmStructure.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay| RTC_AlarmMask_Hours|RTC_AlarmMask_Minutes;

	/* 设置报警时间 */
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x0;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x0;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x5;
	RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
	
	/* 设置RTC中断源 */
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	/* 使能RTC闹钟中断*/
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
	/* 清除闹钟等待 */
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	/* 清除中断线17等待，内存连接至闹钟*/
	EXTI_ClearITPendingBit(EXTI_Line17);

	get_alarm_time();
}

void get_alarm_time(void)
{
	RTC_AlarmTypeDef RTC_AlarmStruct;
	
	RTC_GetAlarm(RTC_Format_BCD,RTC_Alarm_A, &RTC_AlarmStruct);
	printf("The alarm time %02x:%02x:%02x\r\n",RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours,
			RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes,RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds);
}


void RTC_Alarm_IRQHandler(void)
{
	//检查是否指定的中断发生
	if(RTC_GetITStatus(RTC_IT_ALRA)!=RESET)
	{
		printf("alarm interrupt handler:");
		get_rtc_time(); 
		PWR_BackupAccessCmd(ENABLE);//必须有，否则RTC_ClearITPendingBit 不会成功
		RTC_ClearITPendingBit(RTC_IT_ALRA); 
		PWR_BackupAccessCmd(DISABLE); 
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
}
