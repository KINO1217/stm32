#ifndef __QF_DRIVER_CONF_H__
#define __QF_DRIVER_CONF_H__

#include "math.h"
#include "qf_adc.h"
#include "qf_flash.h"
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
// #define QF_MOTOR
// #define QF_HX711
// #define QF_IRRECV
// #define QF_IRSEND
// #define QF_MARTIX_KEY

// #define QF_OLED
// #define QF_MPU6050
// #define QF_MAX30102

// #define QF_ESP8266
// #define QF_SUI101A
// #define QF_GPS
// #define QF_SIM900A
// #define QF_AS608
// #define QF_TCPSERVER

// #define QF_SERVO

// #define QF_RC522
// #define QF_W25QXX

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

#ifdef QF_MOTOR
#include "qf_motor.h"
#endif

#ifdef QF_HX711
#include "qf_hx711.h"
#endif

#ifdef QF_IRRECV
#include "qf_ir_recv.h"
#endif

#ifdef QF_IRSEND
#include "qf_ir_send.h"
#endif

#ifdef QF_MARTIX_KEY
#include "qf_martix_key.h"
#endif

#ifdef QF_OLED
#include "qf_oled.h"
#endif

#ifdef QF_MPU6050
#include "qf_mpu6050.h"
#endif

#ifdef QF_MAX30102
#include "qf_max30102.h"
#endif

#ifdef QF_ESP8266
#include "qf_esp8266.h"
#endif

#ifdef QF_SUI101A
#include "qf_sui101a.h"
#endif

#ifdef QF_GPS
#include "qf_gps.h"
#endif

#ifdef QF_SIM900A
#include "qf_sim900a.h"
#endif

#ifdef QF_AS608
#include "qf_as608.h"
#endif

#ifdef QF_TCPSERVER
#include "qf_tcpserver.h"
#endif

#ifdef QF_SERVO
#include "qf_servo.h"
#endif

#ifdef QF_RC522
#include "qf_rc522.h"
#endif

#ifdef QF_W25QXX
#include "qf_w25qxx.h"
#endif

#endif
