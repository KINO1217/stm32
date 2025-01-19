#ifndef __QF_ESP8266_H__
#define __QF_ESP8266_H__

#include "stm32f10x.h"

/**
 * 使用步骤
 * QF_ESP8266_WifiInit(); // 初始化串口连接WIFI
 * QF_ESP8266_ConnMqtt(); // 连接ONENET服务器
 *
 * 发送数据：
    char send_buf[200] = { 0 };
    char one_buf[64] = { 0 };
    int send_cnt = 0;

    if (send_cnt++ >= 400) {
        if (QF_ESP8266_IsConnected()) {
            memset(send_buf, 0, sizeof(send_buf));

            memset(one_buf, 0, sizeof(one_buf));
            sprintf(one_buf, INTDATA, "t", 0);
            strcat(send_buf, one_buf);

            memset(one_buf, 0, sizeof(one_buf));
            sprintf(one_buf, LASTFLOATDATA, "lat", 15.6);
            strcat(send_buf, one_buf);

            QF_ESP8266_SendData(send_buf);
            send_cnt = 0;
        }
    }
 *
 * 接收数据：
    QF_ESP8266_Callback = esp_call;

    void esp_call(u8* data){
		char* ptr = NULL;
		
		if((ptr = strstr((char*)data,"d1"))!=NULL){
			ptr+=4;
			sscanf(ptr,"%d",&d);
		}
		
		ptr = NULL;
        return;
    }
 */

#define ESP8266_RECV_LENMAX 250

#define WIFI_SSID "test"
#define WIFI_PASSWD "12345678"

#define PRODUCT_ID "9U1Pl6tw1g"
#define TOKEN "version=2018-10-31&res=products%2F9U1Pl6tw1g%2Fdevices%2Fstm32&et=1797558274&method=md5&sign=25zNxnz%2FRXe2tJJE1oMUmQ%3D%3D"
#define DEVICE_ID "stm32"

#define INTDATA "\\\"%s\\\":{\\\"value\\\":%d}\\,"
#define FLOATDATA "\\\"%s\\\":{\\\"value\\\":%f}\\,"
#define STRDATA "\\\"%s\\\":{\\\"value\\\":\\\"%s\\\"}\\,"
#define LASTINTDATA "\\\"%s\\\":{\\\"value\\\":%d}"
#define LASTFLOATDATA "\\\"%s\\\":{\\\"value\\\":%f}"
#define LASTSTRDATA "\\\"%s\\\":{\\\"value\\\":\\\"%s\\\"}"

typedef enum {
    WIFI_CONNECTED = 6,
    ONENET_CONNECTED = 9,
} ESP8266_CONN_STATE;

void QF_ESP8266_WifiInit(void);
void QF_ESP8266_ConnMqtt(void);
void QF_ESP8266_SendData(char* data);
extern void (*QF_ESP8266_Callback)(u8* data);
u8 QF_ESP8266_IsConnected(void);
void QF_USART_Callback(u8* data, u16 data_len);
u8 QF_ESP8266_Verify(char* verify, int timeout);

#endif
