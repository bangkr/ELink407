#ifndef __RTC_H_
#define __RTC_H_
#include "stm32f4xx.h"

struct RtcTime{
	u8 year;
	u8 month;
	u8 date;
	u8 week;
	u8 hour;
	u8 min;
	u8 sec;
};

void rtc_config(void);
void get_rtc_time(void);
void get_alarm_time(void);
void rtc_time_init(void);
void rtc_alarm_config(void);

#endif

