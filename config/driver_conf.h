#ifndef __DRIVER_CONF_H__
#include "clock_conf.h"
#include "string.h"
#include "delay.h"
#include "sys.h"
#include "stdio.h"
#include "my_pwm.h"
#include "my_uart.h"
#include "my_iic.h"
#include "my_timer.h"
#include "my_adc.h"
#include "my_flash.h"

/**********************ALL******************/
//#define SWITCH_DRIVER_ENABLE
//#define KEY_BOARD_DRIVER_ENABLE
//#define DS18B20_DRIVER_ENABLE
//#define DHT11_DRIVER_ENABLE
//#define DS1302_DRIVER_ENABLE
//#define HC_SR04_DRIVER_ENABLE
//#define JQ8400_DRIVER_ENABLE

/*********************IIC*****************/
//#define OLED_DRIVER_ENABLE

/*********************UART*****************/
//#define ESP8266_DRIVER_ENABLE

/*********************PWM*****************/
//#define SERVO_DRIVER_ENABLE

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

#ifdef DS1302_DRIVER_ENABLE
    #include "ds1302.h"
#endif

#ifdef HC_SR04_DRIVER_ENABLE
    #include "hc_sr04.h"
#endif

#ifdef JQ8400_DRIVER_ENABLE
    #include "jq8400.h"
#endif

/*********************IIC*****************/
#ifdef OLED_DRIVER_ENABLE
    #include "oled.h"
#endif

/*********************UART*****************/
#ifdef ESP8266_DRIVER_ENABLE
    #include "esp8266.h"
#endif

/**********PWM*****************/
#ifdef SERVO_DRIVER_ENABLE
    #include "servo.h"
#endif

#endif
