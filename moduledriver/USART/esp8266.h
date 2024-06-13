#ifndef __ESP8266_H__
#include "stm32f10x.h"
#include "my_uart.h"

/**
 * @brief ESP8266 WIFI模块驱动
 * @author KINO
 * @date 2024/4/22
*/

enum WIFI_MODE
{
    AP_MODE=0,  //AP模式
    MQTT_ALIYUN,
    MQTT_ONENET
};

#define WIFI_SSID    "KINO"
#define WIFI_PASSWD  "woshinibaba"
#define PRODUCTKEY    "9U1Pl6tw1g"
#define DEVICEID     "stm32"

typedef struct __ESP8266 c_esp8266;
typedef struct __ESP8266
{
    void *this;
    /**
     * 设置服务器模式回调函数
     * 示例：
        void callback(u8* data,int data_len)
        {
            return;
        }
    */
    void (*setServerCallback)(void (*callback)(u8 *data,int data_len));
    /**
     * AP服务器模式发送数据
     * 示例：
        esp.serverSend(&esp,"123",3);
    */
    void (*serverSend)(const c_esp8266 *this,u8 *data,int data_len);
    /**
     * MQTT物联网上报数据
     * 示例：
        esp.mqttSend(&esp,(u8*)"\\\"temp\\\":{\\\"value\\\":25}\\,\\\"humi\\\":{\\\"value\\\":60}");// ONENET
        esp.mqttSend(&esp,(u8*)"\\\"temp\\\":25\\,\\\"humi\\\":60");// 阿里云
    */
    void (*mqttSend)(const c_esp8266 *this,u8 *data);
    /**
     * MQTT物联网下发数据
     * 示例：
        void mqtt_callback(u8 *key,u8 *value)
        {
            return;
        }

        esp.setMqttCallback(mqtt_callback);
    */
    void (*setMqttCallback)(void (*mqttCallback)(u8 *key,u8 *value));
}c_esp8266;

/**
 * 创建c_esp8266对象
 * @param uart_num 串口号
 * @param mode 模式
 * @param clientID 客户端ID
 * @param username 用户名
 * @param passwd 密码
 * @return c_esp8266对象
 * 示例：
    c_esp8266 esp = {0};

    //AP模式
    esp = esp8266_create(
        MY_UART_ID_USART1,
        AP_MODE,
        NULL,
        "APMODE1",
        "12345678");

    //ALIYUN MQTT模式（需要更改PRODUCTKEY和DEVICEID宏定义）
    esp = esp8266_create(
        MY_UART_ID_USART1,
        MQTT_ALIYUN,
        (u8*)"hv35Sj2mf8c.test001|securemode=2\\,signmethod=hmacsha256\\,timestamp=1718258907761|",
        (u8*)"test001&hv35Sj2mf8c",
        (u8*)"e5ef904d0387b0ef5863d7802dc4e5e4f177ae941b1c223013534f301fbb2835");

    //ONENET MQTT模式（需要更改PRODUCTKEY和DEVICEID宏定义）
    esp = esp8266_create(
        MY_UART_ID_USART1,
        MQTT_ONENET,
        (u8*)"stm32",
        (u8*)"9U1Pl6tw1g",
        (u8*)"version=2018-10-31&res=products%2F9U1Pl6tw1g%2Fdevices%2Fstm32&et=1734449999&method=md5&sign=lxLODkyPEgXjj0yacC38gQ%3D%3D");
*/
c_esp8266 esp8266_create(u8 uart_num,u8 mode,u8 *clientID,u8 *username,u8 *passwd);

#endif
