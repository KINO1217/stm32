#ifndef __QF_DRIVER_CONF_H__
#define __QF_DRIVER_CONF_H__

#include "math.h"
#include "qf_adc.h"
#include "qf_pwm.h"
#include "qf_timer.h"
#include "qf_uart.h"
#include "stdio.h"
#include "stdlib.h"
#include "stm32f10x.h"
#include "string.h"

/**
 * 驱动文件定义
 */
// #define QF_GPIO
// #define QF_DS18B20
// #define QF_DHT11
// #define QF_DS1302
// #define QF_HCSR04

// #define QF_OLED

// #define QF_ESP8266

// #define QF_SERVO

/**
 * 驱动文件引入
 */
#ifdef QF_GPIO
#include "qf_gpio.h"
#endif

#ifdef QF_DS18B20
#include "qf_ds18b20.h"
#endif

#ifdef QF_DHT11
#include "qf_dht11.h"
#endif

#ifdef QF_DS1302
#include "qf_ds1302.h"
#endif

#ifdef QF_HCSR04
#include "qf_hc_sr04.h"
#endif

#ifdef QF_OLED
#include "qf_oled.h"
#endif

#ifdef QF_ESP8266
#include "qf_esp8266.h"
#endif

#ifdef QF_SERVO
#include "qf_servo.h"
#endif

#endif
