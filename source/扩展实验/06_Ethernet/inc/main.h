#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "delay.h"
#include "timer.h"
#include "uart.h"
#include "led.h"
//#include "key.h"
//#include "lcd.h"
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "httpd.h"
#include "serial_debug.h"
#include "stm32f4x7_eth_bsp.h"

#define USE_LCD
#define RMII_MODE
#define PHY_CLOCK_MCO
#define USE_DHCP

#define MAC_ADDR0   2
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0
//Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   10
//NETMASK
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
//Gateway Address
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1  

#endif


