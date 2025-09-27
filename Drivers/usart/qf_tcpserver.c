#include "qf_tcpserver.h"
#include "qf_delay.h"
#include "qf_uart.h"

u8 tcpClientConnected = 0;
u8 setCode=0;
char TCPSERVER_Buff[TCPSERVER_RECV_LENMAX];
u8 TCPSERVER_RecvFlag = 0;
u8 QF_TCPSERVER_Verify(char* verify, int timeout);
void (*QF_TCPSERVER_Callback)(u8* data,u16 data_len);

void QF_RECV_Callback(u8* data,u16 data_len)
{
    if(setCode<9){
        memset(TCPSERVER_Buff, 0, sizeof(TCPSERVER_Buff));
        memcpy(TCPSERVER_Buff, data, data_len);
        TCPSERVER_RecvFlag = 1;
    }else if(setCode==9){ // 解析客户端数据和判断是否有客户端连接
        if(strstr((char*)data,"0,CONNECT\r\n")!=NULL){// 客户端连接
            tcpClientConnected=1;
        }else if(strstr((char*)data,"0,CLOSED\r\n")!=NULL){ //客户端断开
            tcpClientConnected=0;
        }else{
            char* ptr=NULL;
            char recvs[64]={0};
            int recv_len=0;
            int clientNum = 0;
            
            if((ptr=strstr((char*)data,"+IPD"))!=NULL){ // 获取客户端发送的数据
                sscanf(ptr,"+IPD,%d,%d:%[^\r\n]\r\n",&clientNum,&recv_len,recvs);
                QF_TCPSERVER_Callback((u8*)recvs,(u16)recv_len);
            }
            
            ptr=NULL;
        }
    }
}

void QF_TCPSERVER_Init(void)
{
    QF_UART3_Init(115200);
    UART3_Callback = QF_RECV_Callback; // 搜索替换时注意这两个函数名
    
    QF_UART_Printf(USART3, (u8*)"AT\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+RST\r\n"); // 重启设备
    while (QF_TCPSERVER_Verify("AT+RST\r\n\r\nOK\r\n", 5000)) {
        QF_UART_Printf(USART3, (u8*)"AT+RST\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"ATE0\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"ATE0\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CWMODE=2\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CWMODE=2\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CWSAP=\"ESP8266\",\"12345678\",5,2,1,0\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CWSAP=\"ESP8266\",\"12345678\",5,2,1,0\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CWDHCP=0,1\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CWDHCP=0,1\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CIPAP=\"192.168.0.1\",\"192.168.0.1\",\"255.255.255.0\"\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CIPAP=\"192.168.0.1\",\"192.168.0.1\",\"255.255.255.0\"\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CIPMUX=1\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CIPMUX=1\r\n");
    };
    setCode++;
    
    QF_UART_Printf(USART3, (u8*)"AT+CIPSERVER=1,8080\r\n");
    while (QF_TCPSERVER_Verify("\r\nOK\r\n", 2000)) {
        QF_UART_Printf(USART3, (u8*)"AT+CIPSERVER=1,8080\r\n");
    };
    setCode++;
}

void QF_TCPSERVER_SendString(u8* data,u16 data_len)
{    
    QF_UART_Printf(USART3, (u8*)"AT+CIPSEND=0,%d\r\n",data_len);
    QF_DELAY_Ms(100);
    QF_UART_Printf(USART3, (u8*)data);
}

u8 QF_TCPSERVER_Verify(char* verify, int timeout)
{
    u8 ret = 1;
    int cnt = 0;

    while (!TCPSERVER_RecvFlag && cnt < timeout / 10) {
        QF_DELAY_Ms(10);
        cnt++;
    }

    if (strstr(TCPSERVER_Buff, verify) && TCPSERVER_RecvFlag)
        ret = 0;

    TCPSERVER_RecvFlag = 0;

    return ret;
}
