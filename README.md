## 建立初始版本 ##
###主要目录及说明###

ELink407  
├── datasheet  
│   ├── AT24C64.pdf  
│   ├── CS4334.pdf  
│   ├── LAN8720A.pdf  
│   ├── lm358dr.pdf  
│   ├── LM358.pdf  
│   ├── LX T30WH006T1插口规格书.pdf  
│   ├── MPX5050.pdf  
│   ├── PL2303.pdf  
│   ├── SST25VF016B.pdf  
│   ├── STM32F103C8T6.pdf  
│   └── STM32F407VGT6.pdf  
├── docs  
│   ├── 0-preface1.markdown  
│   ├── 1-chapter00-硬件资源.markdown  
│   ├── 1-chapter01-开发工具使用.markdown  
│   ├── 1-chapter02-开发环境搭建.markdown  
│   ├── 1-chapter03-内存映射.markdown  
│   ├── 1-chapter04-启动过程.markdown  
│   ├── 1-chapter05-时钟系统.markdown  
│   ├── 1-chapter06-GPIO.markdown  
│   ├── 1-chapter07-BUZZER.markdown  
│   ├── 1-chapter08-BUTTON.markdown  
│   ├── 1-chapter09-EXTI.markdown  
│   ├── 1-chapter10-SYSTICH.markdown  
│   ├── 1-chapter11-NVIC.markdown  
│   ├── 1-chapter12-uart.markdown  
│   ├── 1-chapter13-DMA.markdown  
│   ├── 1-chapter14-IWDG.markdown  
│   ├── 1-chapter15-WWDG.markdown  
│   ├── 1-chapter16-ADC.markdown  
│   ├── 1-chapter17-I2C.markdown  
│   ├── 1-chapter18-SPI.markdown  
│   ├── 1-chapter19-rtc.markdown  
│   ├── 1-chapter20-timer.markdown    
│   ├── 1-chapter21-pwm.markdown  
│   ├── 1-chapter22-DAC.markdown  
│   ├── 1-chapter24-LCD.markdown  
│   ├── 1-chapter25-显示汉字.markdown    
│   ├── 1-chapter26-显示图片.markdown  
│   ├── 1-chapter27-触摸屏.markdown  
│   ├── 1-chapter28-wakeup.markdown  
│   ├── 1-chapter29-RNG.markdown  
│   ├── 1-chapter30-CRC.markdown  
│   ├── 1-chapter31-CPU-UID.markdown  
│   ├── 1-chapter32-DHT11.markdown  
│   ├── 2-appendix1-sample.markdown  
│   ├── 2-appendix2-cc2git.markdown  
│   ├── ARM MDK安装简要说明.pdf  
│   ├── img  
│   │   ├── chapter0  
│   │   ├── chapter01  
│   │   ├── chapter02   
|   │   ├── chapter03  
│   │   ├── chapter05  
│   │   ├── chapter06  
│   │   ├── chapter10  
│   │   ├── chapter13  
│   │   ├── chapter14  
│   │   ├── chapter15  
│   │   ├── chapter24  
│   │   ├── chapter25  
│   │   ├── chapter26  
│   │   ├── chapter29  
│   │   ├── chapter30  
│   │   ├── chapter31   
│   │   ├── chapter9  
│   │   └── elink407  
│   ├── img-source   
│   └── sdcamp.pdf  
├── SCH  
│   ├── Audio4334.pdf  
│   ├── AudioWM8978.pdf  
│   ├── Elink407V1.0.pdf   
│   ├── Elink407V1500312.pdf  
│   ├── Ethernet.pdf  
│   └── lcdV0.2.pdf   
├── source  
│   ├── STM32Lib  
│   │   ├── STM32F4x7ETH-Driver  
│   │   │   ├──inc  
│   │   │   ├── Release-Notes.html  
│   │   │   └── src  
│   │   ├── STM32F4xx-DSP-StdPeriph-Lib  
│   │   │   ├── -htmresc  
│   │   │   ├── Libraries   
│   │   │   ├── MCD-ST Liberty SW License Agreement V2.pdf  
│   │   │   ├── Project  
│   │   ├── STM32-USB-Device-Library   
│   │   │   ├── Class  
│   │   │   ├── Core  
│   │   │   └── Release-Notes.html  
│   │   ├── STM32-USB-HOST-Library  
│   │   │   ├── Class  
│   │   │   ├── Core  
│   │   │   └── Release-Notes.html  
│   │   └── STM32-USB-OTG-Driver  
│   │       ├── inc  
│   │       ├── ReleaseNotes.html  
│   │       └── src  
│   ├── ucosIII  
│   ├── 基础实验  
│   │   ├── bsp4Elink  
│   │   │   ├── inc  
│   │   │   └── src  
│   │   ├── common  
│   │   │   ├── font  
│   │   │   ├── IAR  
│   │   │   ├── img  
│   │   │   ├── MDK  
│   │   │   └── __一些公共的sys文件  
│   │   ├── keilkill.bat  
│   │   └── project  
│   │       ├── 03_project  
│   │       ├── 06_led_nolib  
│   │       ├── 07_led  
│   │       ├── 08_buzzer  
│   │       ├── 09_button_noexti  
│   │       ├── 10_button  
│   │       ├── 11_uart  
│   │       ├── 12_uart_DMA  
│   │       ├── 13_systick  
│   │       ├── 14_NVIC  
│   │       ├── 15_IWDG  
│   │       ├── 16_WWDG  
│   │       ├── 17_ADC  
│   │       ├── 18_IIC  
│   │       ├── 19_SPI  
│   │       ├── 20_RTC  
│   │       ├── 21_TIMER  
│   │       ├── 22_PWM  
│   │       ├── 23_DAC  
│   │       ├── 24_LCD  
│   │       ├── 25_LCD _font  
│   │       ├── 26_LCD _img  
│   │       ├── 28_WakeUp  
│   │       ├── 29_RNG  
│   │       ├── 30_CRC  
│   │       ├── 31_CPU_UID  
│   │       └── 32_DHT11  
│   └── 扩展实验  
│       ├── 01_rs485  
│       │   ├── board  
│       │   ├── inc  
│       │   ├── proj  
│       │   └── src  
│       ├── 02_CAN  
│       │   ├── board  
│       │   ├── proj  
│       │   └── user  
│       ├── 03_SD  
│       │   ├── board   
│       │   ├── MALLOC   
│       │   ├── proj  
│       │   └── user  
│       ├── 04_audio_4334  
│       │   ├── board  
│       │   ├── inc  
│       │   ├── proj  
│       │   ├── src  
│       │   └── Third_Party  
│       ├── 05_audio_wm8978  
│       │   ├── APP  
│       │   ├── AUDIOCODEC  
│       │   ├── board  
│       │   ├── FATFS  
│       │   ├── FWLIB  
│       │   ├── MALLOC  
│       │   ├── proj  
│       │   ├── TEXT  
│       │   └── USER  
│       ├── 06_Ethernet  
│       │   ├── board  
│       │   ├── inc  
│       │   ├── proj  
│       │   ├── src  
│       │   └── Third_Party  
│       ├── bsp4Elink  
│       │   ├── inc  
│       │   └── src  
│       ├── common  
│       │   ├── font  
│       │   ├── IAR  
│       │   ├── img  
│       │   ├── MDK  
│       │   └── __一些公共的sys文件  
│       └── keilkill.bat  
├── tools  
│   ├── arm mdk  
│   │   ├── Keil.STM32F4xxDFP.1.0.6.pack  
│   │   ├── keygen.exe  
│   │   ├── keygen.zip  
│   │   └── mdk510.exe   
│   ├── elinklogo.bmp   
│   ├── Image2Lcd.rar  
│   ├── putty.exe  
│   ├── SecureCRT.rar  
│   ├── stlink2  
│   │   ├── drivers  
│   │   │   ├── stsw-link008.zip  
│   │   │   └── stsw-link009.zip   
│   │   ├── STM32 ST-LINK Utilityv3.3.0.exe   
│   │   └── stsw-link004.zip  
│   └── usb2uart driver  
│       ├── PL2303ProlificriverInstaller19WinXP）.zip  
│       └── PL-2303 Vista Driver Installer（Win7、8）.zip  
└── 资源  
    ├── stm32cubef4.zip  
    ├── stm32_f105-07_f2_f4_usb-host-device_lib.zip  
    └── stsw-stm32069.zip  
