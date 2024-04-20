#ifndef __DRIVER_CONF_H__
#include "clock_conf.h"
#include "string.h"
#include "delay.h"
#include "sys.h"
#include "stdio.h"
#include "my_uart.h"
#include "my_iic.h"
/**********************ALL******************/
//#define SWITCH_DRIVER_ENABLE
//#define KEY_BOARD_DRIVER_ENABLE
//#define DS18B20_DRIVER_ENABLE
//#define DHT11_DRIVER_ENABLE

/*********************IIC*****************/
//#define OLED_DRIVER_ENABLE

/**********************ALL******************/
#ifdef SWITCH_DRIVER_ENABLE
    #include "switch.h"
#endif

#ifdef KEY_BOARD_DRIVER_ENABLE
    #include "key_board.h"
#endif

#ifdef DS18B20_DRIVER_ENABLE
    #include "ds18b20.h"
#endif

#ifdef DHT11_DRIVER_ENABLE
    #include "dht11.h"
#endif

/*********************IIC*****************/
#ifdef OLED_DRIVER_ENABLE
    #include "oled.h"
#endif

#endif
