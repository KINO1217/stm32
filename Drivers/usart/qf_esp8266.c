#include "qf_esp8266.h"
#include "qf_delay.h"
#include "qf_uart.h"

u8 connCode = 0;
char esp8266_recvBuf[ESP8266_RECV_LENMAX];
u8 recv_flag = 0;
void (*QF_ESP8266_Callback)(u8* data);

void QF_USART_Callback(u8* data, u16 data_len)
{
    if (connCode < ONENET_CONNECTED) {
        memset(esp8266_recvBuf, 0, sizeof(esp8266_recvBuf));
        memcpy(esp8266_recvBuf, data, data_len);
        recv_flag = 1;
    } else { // 处理远程下发属性
        char* ptr = NULL;
        if ((ptr = strstr((char*)data, "+MQTTSUBRECV")) != NULL) {
            ptr = strstr((char*)ptr, "params");
            ptr += 9;
            sscanf(ptr, "%[^}]}", ptr);

            QF_ESP8266_Callback((u8*)ptr);

            ptr = NULL;
        }
    }
}

void QF_ESP8266_WifiInit(void)
{
    QF_UART3_Init(115200);
    UART3_Callback = QF_USART_Callback; // 搜索替换时注意这两个函数名

    QF_UART_Printf(USART3, (u8*)"AT+RST\r\n"); // 重启设备
    while (QF_ESP8266_Verify("AT+RST\r\n\r\nOK\r\n", 3000)) {
        QF_UART_Printf(USART3, (u8*)"AT+RST\r\n");
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"ATE0\r\n"); // 关闭回显
    while (QF_ESP8266_Verify("ATE0\r\n\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"ATE0\r\n");
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+CWMODE=1\r\n"); // 设置STA模式
    while (QF_ESP8266_Verify("\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"AT+CWMODE=1\r\n");
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWD); // 连接WIFI
    while (QF_ESP8266_Verify("\r\nOK\r\n", 3000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWD);
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+CIPMODE=1\r\n"); // 透传模式
    while (QF_ESP8266_Verify("\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"AT+CIPMODE=1\r\n");
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+CIPMUX=0\r\n"); // 关闭多连接
    while (QF_ESP8266_Verify("\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"AT+CIPMUX=0\r\n");
    };
    connCode++;
}

void QF_ESP8266_ConnMqtt(void)
{
    QF_UART_Printf(USART3, (u8*)"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", DEVICE_ID, PRODUCT_ID, TOKEN); // 配置MQTT连接参数
    while (QF_ESP8266_Verify("\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", DEVICE_ID, PRODUCT_ID, TOKEN);
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n"); // 连接onenet
    while (QF_ESP8266_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,0\r\n");
    };
    connCode++;

    QF_UART_Printf(USART3, (u8*)"AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",0\r\n", PRODUCT_ID, DEVICE_ID); // 订阅set主题
    while (QF_ESP8266_Verify("\r\nOK\r\n", 500)) {
        QF_UART_Printf(USART3, (u8*)"AT+MQTTSUB=0,\"$sys/%s/%s/thing/property/set\",0\r\n", PRODUCT_ID, DEVICE_ID);
    };
    connCode++;
}

void QF_ESP8266_SendData(char* data)
{
    QF_UART_Printf(USART3, (u8*)"AT+MQTTPUB=0,\"$sys/%s/%s/thing/property/post\",\"{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{%s}}\",0,0\r\n", PRODUCT_ID, DEVICE_ID, data);
}

u8 QF_ESP8266_IsConnected(void)
{
    return (connCode == ONENET_CONNECTED ? 1 : 0);
}

u8 QF_ESP8266_Verify(char* verify, int timeout)
{
    u8 ret = 1;
    int cnt = 0;

    while (!recv_flag && cnt < timeout / 10) {
        QF_DELAY_Ms(10);
        cnt++;
    }

    if (strstr(esp8266_recvBuf, verify) && recv_flag)
        ret = 0;

    recv_flag = 0;

    return ret;
}
