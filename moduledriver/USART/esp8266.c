#include "esp8266.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "delay.h"

/**
 * AP模式指令集
 */
#define EXIT_CIPSEND "+++"                                                     // 退出发送数据
#define ATRST "AT+RST\r\n"                                                     // 复位
#define ATE0 "ATE0\r\n"                                                        // 关闭回显
#define CWMODE2 "AT+CWMODE=2\r\n"                                              // 设置工作模式为AP模式
#define CWSAP "AT+CWSAP=\"%s\",\"%s\",5,2,1,0\r\n"                             // 设置AP参数
#define CIPAP "AT+CIPAP=\"192.168.0.1\",\"192.168.0.1\",\"255.255.255.0\"\r\n" // 设置AP信息
#define CIPMUX "AT+CIPMUX=1\r\n"                                               // 多连接模式
#define CIPSERVER "AT+CIPSERVER=1,8080\r\n"                                    // 开启服务器
#define SERCIPSEND "AT+CIPSEND=0,%d\r\n"                                       // 发送数据

/**
 * MQTT指令集
 */
#define CWMODE1 "AT+CWMODE=1\r\n"          // 设置工作模式为STA模式
#define CWJAP "AT+CWJAP=\"%s\",\"%s\"\r\n" // 连接到AP
#define MQTTUSERCFG "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n"
#define MQTTONENETCONN "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n"
#define MQTTONENETSUB "AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",0\r\n"
#define MQTTONENETPUB "AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{%s}}\",0,0\r\n"
#define MQTTALIYUNCONN "AT+MQTTCONN=0,\"iot-06z00ahw0e1vcqn.mqtt.iothub.aliyuncs.com\",1883,0\r\n"
#define MQTTLIYUNPUB "AT+MQTTPUB=0,\"/sys/%s/%s/thing/event/property/post\",\"{\\\"params\\\":{%s}}\",0,0\r\n"
#define MQTTCLEAN "AT+MQTTCLEAN=0\r\n"

typedef struct __MESP8266
{
    u8 uart_num;
} m_esp;

/**
 * 全局变量+函数定义
 */
int recvFlag = 0;
char recvBuff[189];
void espCallback(u8 *data, int len);
int connCode = 0; // 连接码
u8 errorCnt = 0;  // 校验错误次数
void apSet(u8 uart_num, u8 *username, u8 *passwd);
void verify(int time, u8 *verify);
char sendBuff[216];
void setServerCallback(void (*callback)(u8 *data, int data_len));
void (*serverCallback)(u8 *data, int data_len);
u8 currMode = AP_MODE; // 模式
void serverSend(const c_esp8266 *this, u8 *data, int data_len);
void mqttSet(u8 uart_num, u8 *clientId, u8 *username, u8 *password);
void mqttSend(const c_esp8266 *this, u8 *data);
void setMqttCallback(void (*callback)(u8 *key,u8 *value));
void (*mqttCallback)(u8 *key,u8 *value);

c_esp8266 esp8266_create(u8 uart_num, u8 mode, u8 *clientID, u8 *username, u8 *passwd)
{
    /**创建c_esp8266对象*/
    c_esp8266 new = {0};

    /*初始化串口*/
    my_uart.init(uart_num, 115200, ISR_MODE);
    my_uart.setCallback(uart_num, espCallback);

    /**设置M_ESP结构体*/
    new.this = malloc(sizeof(m_esp));
    memset(new.this, 0, sizeof(m_esp));
    ((m_esp *)new.this)->uart_num = uart_num;
    new.setServerCallback = setServerCallback;
    currMode = mode;
    new.serverSend = serverSend;
    new.mqttSend = mqttSend;
    new.setMqttCallback = setMqttCallback;

    if (mode == AP_MODE)
    {
        apSet(uart_num, username, passwd);
    }
    else if (mode == MQTT_ALIYUN || mode == MQTT_ONENET)
    {
        mqttSet(uart_num, clientID, username, passwd);
    }

    return new;
}

/**
 * 串口回调函数
 */
void espCallback(u8 *data, int len)
{
    memset(recvBuff, 0, sizeof(recvBuff));
    memcpy(recvBuff, data, len);
    recvFlag = 1;

    if ((connCode == 8 || connCode == 9) && currMode == AP_MODE) // 连接完成
    {
        char *s = NULL;
        if (strstr(recvBuff, "CONNECT\r\n"))
        {
            connCode++;
        }
        else if (strstr(recvBuff, "CLOSED\r\n"))
        {
            connCode--;
        }
        else if ((s = strstr(recvBuff, "+IPD")) != NULL)
        {
            int recvLen = 0;
            char recvData[64] = {0};
            sscanf(s, "+IPD,0,%d:%s", &recvLen, recvData);
            serverCallback((u8 *)recvData, recvLen);
        }
        recvFlag = 0;
    }
    else if (connCode == 7 && (currMode == MQTT_ALIYUN || currMode == MQTT_ONENET))
    {
        char *s = NULL;
        if ((s = strstr(recvBuff, "+MQTTSUBRECV:")) != NULL)
        {
            char *s1 = NULL;
            if ((s1 = strstr(s, "/property/set")) != NULL)
            {
                char *s2 = NULL;
                if((s2=strstr(s1,"\"params\""))!=NULL)
                {
                    char key[16];
                    char value[16];
                    sscanf(s2,"\"params\":{\"%[^\"]\":%[^}]}",key,value);
                    mqttCallback((u8*)key,(u8*)value);
                }
            }
        }
        recvFlag = 0;
    }
}

/**
 * AP指令设置
 */
void apSet(u8 uart_num, u8 *username, u8 *passwd)
{
    while (1)
    {
        switch (connCode)
        {
        case 0:
            my_uart.send(uart_num, (u8 *)EXIT_CIPSEND, strlen(EXIT_CIPSEND));
            vDelayMs(1000);
            connCode++;
            break;
        case 1:
            my_uart.send(uart_num, (u8 *)ATRST, strlen(ATRST));
            vDelayMs(2000);
            connCode++;
            break;
        case 2:
            my_uart.send(uart_num, (u8 *)ATE0, strlen(ATE0));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 3:
            my_uart.send(uart_num, (u8 *)CWMODE2, strlen(CWMODE2));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 4:
            memset(sendBuff, 0, sizeof(sendBuff));
            sprintf(sendBuff, CWSAP, username, passwd);
            my_uart.send(uart_num, (u8 *)sendBuff, strlen(sendBuff));
            verify(1500, (u8 *)"OK\r\n");
        case 5:
            my_uart.send(uart_num, (u8 *)CIPAP, strlen(CIPAP));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 6:
            my_uart.send(uart_num, (u8 *)CIPMUX, strlen(CIPMUX));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 7:
            my_uart.send(uart_num, (u8 *)CIPSERVER, strlen(CIPSERVER));
            verify(1000, (u8 *)"OK\r\n");
            break;
        default:
            goto success;
        }
    }

success:
    return;
}

/**
 * 回调数据校验
 */
void verify(int time, u8 *verify)
{
    vDelayMs(time);
    if (recvFlag == 1)
    {
        if (strstr(recvBuff, (char *)verify))
        {
            connCode++;
            errorCnt = 0;
        }
        else
        {
            errorCnt++;
            if (errorCnt > 5)
            {
                errorCnt = 0;
                connCode = 0;
            }
        }
        recvFlag = 0;
    }
}

/**
 * 设置服务器回调函数
 */
void setServerCallback(void (*callback)(u8 *data, int data_len))
{
    serverCallback = callback;
}

/**
 * 服务器发送数据
 */
void serverSend(const c_esp8266 *this, u8 *data, int data_len)
{
    m_esp *m_this = {0};
    m_this = (m_esp *)this->this;

    if (connCode == 9)
    {
        memset(sendBuff, 0, sizeof(sendBuff));
        sprintf(sendBuff, SERCIPSEND, data_len);
        my_uart.send(m_this->uart_num, (u8 *)sendBuff, strlen(sendBuff));
        vDelayMs(150);
        my_uart.send(m_this->uart_num, data, data_len);
        vDelayMs(150);
    }
}

/**
 * MQTT指令设置
 */
void mqttSet(u8 uart_num, u8 *clientId, u8 *username, u8 *password)
{
    while (1)
    {
        switch (connCode)
        {
        case 0:
            my_uart.send(uart_num, (u8 *)ATRST, strlen(ATRST));
            vDelayMs(2000);
            connCode++;
            break;
        case 1:
            my_uart.send(uart_num, (u8 *)ATE0, strlen(ATE0));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 2:
            my_uart.send(uart_num, (u8 *)CWMODE1, strlen(CWMODE1));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 3:
            memset(sendBuff, 0, sizeof(sendBuff));
            sprintf(sendBuff, CWJAP, WIFI_SSID, WIFI_PASSWD);
            my_uart.send(uart_num, (u8 *)sendBuff, strlen(sendBuff));
            verify(3000, (u8 *)"OK\r\n");
            break;
        case 4:
            memset(sendBuff, 0, sizeof(sendBuff));
            sprintf(sendBuff, MQTTUSERCFG, clientId, username, password);
            my_uart.send(uart_num, (u8 *)sendBuff, strlen(sendBuff));
            verify(1000, (u8 *)"OK\r\n");
            break;
        case 5:
            if (currMode == MQTT_ONENET)
            {
                my_uart.send(uart_num, (u8 *)MQTTONENETCONN, strlen(MQTTONENETCONN));
                verify(1000, (u8 *)"OK\r\n");
            }
            else if (currMode == MQTT_ALIYUN)
            {
                my_uart.send(uart_num, (u8 *)MQTTALIYUNCONN, strlen(MQTTALIYUNCONN));
                verify(1000, (u8 *)"OK\r\n");
            }
            break;
        case 6:
            if(currMode==MQTT_ONENET)
            {
                memset(sendBuff, 0, sizeof(sendBuff));
                sprintf(sendBuff, MQTTONENETSUB, PRODUCTKEY, DEVICEID);
                my_uart.send(uart_num, (u8 *)sendBuff, strlen(sendBuff));
                verify(1000, (u8 *)"OK\r\n");
            }
            else if(currMode == MQTT_ALIYUN)
            {
                connCode++;
            }
        default:
            goto success;
        }
    }

success:
    return;
}

/**
 * MQTT发布消息
 */
void mqttSend(const c_esp8266 *this, u8 *data)
{
    m_esp *m_this = {0};
    m_this = (m_esp *)this->this;

    if (connCode == 7 && currMode == MQTT_ALIYUN)
    {
        memset(sendBuff, 0, sizeof(sendBuff));
        sprintf(sendBuff, MQTTLIYUNPUB, PRODUCTKEY, DEVICEID, data);
        my_uart.send(m_this->uart_num, (u8 *)sendBuff, strlen(sendBuff));
    }
    else if (connCode == 7 && currMode == MQTT_ONENET)
    {
        memset(sendBuff, 0, sizeof(sendBuff));
        sprintf(sendBuff, MQTTONENETPUB, PRODUCTKEY, DEVICEID, data);
        my_uart.send(m_this->uart_num, (u8 *)sendBuff, strlen(sendBuff));
    }
}

void setMqttCallback(void (*callback)(u8 *key,u8 *value))
{
    mqttCallback = callback;
}
