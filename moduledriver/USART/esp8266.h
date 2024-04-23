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
    MODE_AP=0,  //AP模式，暂时仅启用此模式
    MQTT_ALIYUN,
    MQTT_ONENET
};

typedef struct __ESP8266 c_esp8266;
typedef struct __ESP8266
{
    void *this;
    /**
     * 创建TCP服务器，while循环中使用
     * 示例：
        esp8266.conn(&esp8266);
    */
    void (*conn)(const c_esp8266 *this);
    /**
     * 设置服务器回调函数，用于接收客户端数据
     * 示例：
        void serverCallback(u8 *data,int dataLen)
        {
            return;
        }

        esp8266.setServerCallback(serverCallback);
    */
    void (*setServerCallback)(void (*callback)(u8 *data,int dataLen));
    /**
     * 向客户端发送消息
     * 示例：
        esp8266.serverSend(&esp8266,data,dataLen);
    */
    void (*serverSend)(const c_esp8266 *this,u8 *data,int dataLen);
}c_esp8266;

/**
 * 创建esp8266对象
 * 示例：
    c_esp8266 esp8266 = {0};

    esp8266 = esp8266_create(MY_UART_ID_USART3, //串口号
            MODE_AP,                            //工作模式
            NULL,                               //客户端ID
            (u8*)"APMode",                      //用户名
            (u8*)"12345678"                     //密码
        );
*/
c_esp8266 esp8266_create(u8 uart_num,u8 mode,u8 *clientID,u8 *username,u8 *passwd);

#endif
