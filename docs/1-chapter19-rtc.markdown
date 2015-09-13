# RTC实验 #

RTC是一个实时时钟,是一个独立的定时器，RTC模块拥有一组连续计数的计数器，在相应软件配置下，可以提供时钟、日历的功能。修改计数器的值可以重新设置系统当前的时间和日期

RTC可以用来设置实时时钟，并产生秒中断或闹钟中断。在芯片有电源供电的情况下，系统掉电后仍能保持时钟计时。

## 实验原理与基础知识 ##
RTC设置需要涉及对电源（PWR）、备份区域（BKP）和RCC部分寄存器的改写，对于RTC的使用，首先我们要判断是否是第一次使用RTC，如果是第一次使用首先要对RTC进行相应的配置，并初始化其时间。如果不是第一次使用，那么我们就无需再设置时间的初始值以及对RTC进行相应的配置，只需让RTC计数器继续计数就可以了。

RTC的时钟源可以选择内部低速时钟LSI、外部低速时钟LSE和时钟HSE，内部时钟LSI不够准确，本案例选择外部时钟LSE。

![](img/chapter0/rtc_sch.png) 

## 电路设计 ##

## 库函数和数据结构 ##

RTC配置直接相关的几个固件库函数，这些函数的定义主要分布在stm32f4xx_rtc.h 和 stm32f4xx_rtc.c 文件中。

使能电源时钟总线：

        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
        
选择rtc晶振（外部晶振LSE）：

        RCC_LSEConfig(RCC_LSE_ON);//使用外部晶振
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择RTC时钟源 
        
开启rtc时钟：

        RCC_RTCCLKCmd(ENABLE);
        
rtc时间数据结构：

        typedef struct
        {
          uint8_t RTC_Hours; 
          uint8_t RTC_Minutes; 
          uint8_t RTC_Seconds; 
          uint8_t RTC_H12; 
        }RTC_TimeTypeDef; 
        
rtc日期数据结构：

        typedef struct
        {
          uint8_t RTC_WeekDay; 
          uint8_t RTC_Month;
          uint8_t RTC_Date;  
          uint8_t RTC_Year; 
        }RTC_DateTypeDef;

rtc闹钟数据结构：

        typedef struct
        {
          RTC_TimeTypeDef RTC_AlarmTime; 
          uint32_t RTC_AlarmMask;   

          uint32_t RTC_AlarmDateWeekDaySel; 
          uint8_t RTC_AlarmDateWeekDay; 
        }RTC_AlarmTypeDef;

读取rtc日期和时间：

        RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure); //读日期
        RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure); //读时间

设置闹钟报警时间：

        RTC_SetAlarm(RTC_Format_BCD,RTC_Alarm_A,&RTC_AlarmStructure);
        
获取rtc闹钟时间：

        RTC_GetAlarm(RTC_Format_BCD,RTC_Alarm_A, &RTC_AlarmStruct);
        
开启rtc闹钟：

        RTC_AlarmCmd(RTC_Alarm_A,ENABLE);

## 软件设计、开发 ##
在工程中添加rtc.c 文件和rtc.h 文件，RTC相关的函数和定义都在这两个文件中。

1)  配置rtc实时时钟

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

2） 初始化设置rtc时间。

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
        
3)  配置rtc闹钟中断

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
            RTC_AlarmStructure.RTC_AlarmMask=RTC_AlarmMask_DateWeekDay| 
                    RTC_AlarmMask_Hours|RTC_AlarmMask_Minutes;

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
        
4)  闹钟中断处理函数

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

5) 编写main函数

        int main(void)
        {	
            /*uart1 configure*/
            uart_config();
            printf(" RTC example\r\n");

            /*Configuration delay millisecond precision*/
            SysTick_Configuration();

            /*configuration rtc*/
            rtc_time_init();

            /*configuration alarm time*/
            rtc_alarm_config();//必须在rtc初始化之后

            //get current time
            while(1){
                delay_ms(500);
                delay_ms(500);
                get_rtc_time();
                //get_alarm_time();
            }
        }

## 软件下载、测试验证 ##


