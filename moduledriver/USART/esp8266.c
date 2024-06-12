#include "esp8266.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "delay.h"

/**
 * AP模式指令集
 */
#define EXIT_CIPSEND    "+++"                       //退出发送数据
#define ATRST "AT+RST\r\n"                          //复位
#define ATE0 "ATE0\r\n"                             //关闭回显
#define CWMODE2 "AT+CWMODE=2\r\n"                   //设置工作模式为AP模式
#define CWSAP "AT+CWSAP=\"%s\",\"%s\",5,2,1,0\r\n"  //设置AP参数
#define CIPAP "AT+CIPAP=\"192.168.0.1\",\"192.168.0.1\",\"255.255.255.0\"\r\n"                        //设置AP信息
#define CIPMUX "AT+CIPMUX=1\r\n"                     //多连接模式
#define CIPSERVER "AT+CIPSERVER=1,8080\r\n"          //开启服务器
#define SERCIPSEND "AT+CIPSEND=0,%d\r\n"             //发送数据

typedef struct __MESP8266
{
    u8 uart_num;
}m_esp;

/**
 * 全局变量+函数定义
*/
int recvFlag=0;
char recvBuff[64];
void espCallback(u8 *data, int len);
int connCode = 0; //连接码
u8 errorCnt = 0; //校验错误次数
void apSet(u8 uart_num, u8 *username, u8 *passwd);
void apVerify(int time,u8 *verify);
char sendBuff[64];
void setServerCallback(void (*callback)(u8 *data,int data_len));
void (*serverCallback)(u8 *data,int data_len);
u8 currMode = AP_MODE; // 模式
void serverSend(const c_esp8266 *this, u8 *data, int data_len);

c_esp8266 esp8266_create(u8 uart_num, u8 mode, u8 *clientID, u8 *username, u8 *passwd)
{
    /**创建c_esp8266对象*/
    c_esp8266 new = {0};

    /*初始化串口*/
    my_uart.init(uart_num,115200,ISR_MODE);
    my_uart.setCallback(uart_num,espCallback);

    /**设置M_ESP结构体*/
    new.this = malloc(sizeof(m_esp));
    memset(new.this,0,sizeof(m_esp));
    ((m_esp*)new.this)->uart_num = uart_num;
    new.setServerCallback = setServerCallback;
    currMode = mode;
    new.serverSend = serverSend;

    if(mode == AP_MODE)
    {
        apSet(uart_num,username,passwd);
    }

    return new;
}

/**
 * 串口回调函数
*/
void espCallback(u8 *data, int len)
{
    memset(recvBuff,0,sizeof(recvBuff));
    memcpy(recvBuff,data,len);
    recvFlag=1;

    if((connCode == 8 || connCode == 9) && currMode == AP_MODE) //连接完成
    {
        char *s = NULL;
        if(strstr(recvBuff,"CONNECT\r\n"))
        {
            connCode++;
        }
        else if(strstr(recvBuff,"CLOSED\r\n"))
        {
            connCode--;
        }
        else if((s = strstr(recvBuff,"+IPD"))!=NULL)
        {
            int recvLen = 0;
            char recvData[64] = {0};
            sscanf(s,"+IPD,0,%d:%s",&recvLen,recvData);
            serverCallback((u8*)recvData,recvLen);
        }
        recvFlag=0;
    }
}

/**
 * AP指令设置
*/
void apSet(u8 uart_num, u8 *username, u8 *passwd)
{
    while(1)
    {
        switch(connCode)
        {
            case 0:
                my_uart.send(uart_num,(u8*)EXIT_CIPSEND,strlen(EXIT_CIPSEND));
                vDelayMs(1000);
                connCode++;
                break;
            case 1:
                my_uart.send(uart_num,(u8*)ATRST,strlen(ATRST));
                vDelayMs(2000);
                connCode++;
                break;
            case 2:
                my_uart.send(uart_num,(u8*)ATE0,strlen(ATE0));
                apVerify(1000,(u8*)"OK\r\n");
                break;
            case 3:
                my_uart.send(uart_num,(u8*)CWMODE2,strlen(CWMODE2));
                apVerify(1000,(u8*)"OK\r\n");
                break;
            case 4:
                memset(sendBuff,0,sizeof(sendBuff));
                sprintf(sendBuff,CWSAP,username,passwd);
                my_uart.send(uart_num,(u8*)sendBuff,strlen(sendBuff));
                apVerify(1500,(u8*)"OK\r\n");
            case 5:
                my_uart.send(uart_num,(u8*)CIPAP,strlen(CIPAP));
                apVerify(1000,(u8*)"OK\r\n");
                break;
            case 6:
                my_uart.send(uart_num,(u8*)CIPMUX,strlen(CIPMUX));
                apVerify(1000,(u8*)"OK\r\n");
                break;
            case 7:
                my_uart.send(uart_num,(u8*)CIPSERVER,strlen(CIPSERVER));
                apVerify(1000,(u8*)"OK\r\n");
                break;
            default:
                goto success;
        }
    }
    
    success:
    return;
}

/**
 * AP模式回调校验
*/
void apVerify(int time,u8 *verify)
{
    vDelayMs(time);
    if(recvFlag==1)
    {
        if(strstr(recvBuff,(char*)verify))
        {
            connCode++;
            errorCnt=0;
        }
        else 
        {
            errorCnt++;
            if(errorCnt>5)
            {
                errorCnt=0;
                connCode=0;
            }
        }
        recvFlag=0;
    }
}

/**
 * 设置服务器回调函数
*/
void setServerCallback(void (*callback)(u8 *data,int data_len))
{
    serverCallback = callback;
}

/**
 * 服务器发送数据
*/
void serverSend(const c_esp8266 *this, u8 *data, int data_len)
{
    m_esp *m_this = {0};
    m_this = (m_esp*)this->this;

    if(connCode==9)
    {
        memset(sendBuff,0,sizeof(sendBuff));
        sprintf(sendBuff,SERCIPSEND,data_len);
        my_uart.send(m_this->uart_num,(u8*)sendBuff,strlen(sendBuff));
        vDelayMs(150);
        my_uart.send(m_this->uart_num,data,data_len);
        vDelayMs(150);
    }
}
