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
    AP_MODE=0,  //AP模式，暂时仅启用此模式
    MQTT_ALIYUN,
    MQTT_ONENET
};

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

    esp = esp8266_create(MY_UART_ID_USART1,AP_MODE,NULL,"APMODE1","12345678");
*/
c_esp8266 esp8266_create(u8 uart_num,u8 mode,u8 *clientID,u8 *username,u8 *passwd);

#endif
