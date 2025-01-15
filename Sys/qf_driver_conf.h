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
 * �����ļ�����
 */
// #define QF_GPIO
// #define QF_DS18B20
// #define QF_DHT11

// #define QF_OLED

// #define QF_ESP8266

// #define QF_SERVO

/**
 * �����ļ�����
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
