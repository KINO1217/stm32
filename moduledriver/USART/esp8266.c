#include "esp8266.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

/**
 * AP模式指令集
*/
#define EXIT_SEND_MESSAGE "+++"
#define CLOSE_ATE "ATE0\r\n"
#define ATRST	"AT+RST\r\n"
#define CWMODE_AP "AT+CWMODE=2\r\n"
#define CWSAP_SET "AT+CWSAP=\"%s\",\"%s\",5,2,1,0\r\n"
#define CIPAP_SET "AT+CIPAP=\"192.168.0.1\",\"192.168.0.1\",\"255.255.255.0\"\r\n"
#define CIPMUX_SET "AT+CIPMUX=1\r\n"
#define CIPSERVER_SET "AT+CIPSERVER=1,8080\r\n"
#define CIPSEND_SET "AT+CIPSEND=0,%d\r\n"

typedef struct __MESP8266
{
    u8 uart_num;    //串口号
    u8 connCode;    //连接码
    u8 clientState; //客户端状态
    u8 mode;        //模式
    u8 *clientID;   //客户端ID
    u8 *username;   //用户名
    u8 *passwd;     //密码
    int connTimeCnt; //等待数据返回计数
    u8 sendState;   //发送数据状态
}m_esp;

enum __APVERIFYMODE{
    VERIFY_ADD=0,
    VERIFY_SUB,
    VERIFY_BACK_ZERO
};

static void uart_callback(u8 *data,int len);
char sendBuff[64];
char recvBuff[64];
u8 recvFlag;
static void verifyAP(const c_esp8266 *this,int cnt,u8 verifyMode);
static void toApMode(const c_esp8266 *this);
static void conn(const c_esp8266 *this);
static void getClientMessage(const c_esp8266 *this);
static void setServerCallback(void (*callback)(u8 *data,int dataLen));
static void (*serverCallback)(u8 *data,int dataLen);
static void serverSend(const c_esp8266 *this,u8 *data,int dataLen);

c_esp8266 esp8266_create(u8 uart_num,u8 mode,u8 *clientID,u8 *username,u8 *passwd)
{
    c_esp8266 new={0};

    //初始化串口
    my_uart.init(uart_num,115200,ISR_MODE);
    my_uart.setCallback(uart_num,uart_callback);

    //保存变量
    new.this = malloc(sizeof(m_esp));
    memset(new.this,0,sizeof(m_esp));
    ((m_esp*)new.this)->uart_num = uart_num;
    ((m_esp*)(new.this))->connCode=0;
    ((m_esp*)new.this)->clientState = 0;
    ((m_esp*)new.this)->mode = mode;
    ((m_esp*)new.this)->clientID = clientID;
    ((m_esp*)new.this)->username = username;
    ((m_esp*)new.this)->passwd = passwd;
    ((m_esp*)new.this)->connTimeCnt = 0;
    ((m_esp*)new.this)->sendState = 0;
    new.conn = conn;
    new.setServerCallback = setServerCallback;
    new.serverSend = serverSend;

    return new;
}

static void setServerCallback(void (*callback)(u8 *data,int dataLen))
{
    serverCallback = callback;
}

static void serverSend(const c_esp8266 *this,u8 *data,int dataLen)
{
    m_esp *m_this = {0};
    m_this = (m_esp*)this->this;

    if(m_this->clientState == 1 && m_this->sendState == 0)
    {
        memset(sendBuff,0,sizeof(sendBuff));
        sprintf(sendBuff,CIPSEND_SET,dataLen);
        my_uart.send(m_this->uart_num,(u8*)sendBuff,strlen(sendBuff));

        m_this->sendState = 1;
        memset(sendBuff,0,sizeof(sendBuff));
        memcpy(sendBuff,data,dataLen);
    }
}

static void uart_callback(u8 *data,int len)
{
    memset(recvBuff,0,sizeof(recvBuff));
    memcpy(recvBuff,data,len);
    recvFlag = 1;
}

static void conn(const c_esp8266 *this)
{
    m_esp *m_this = {0};
    m_this = this->this;

    if(m_this->mode == MODE_AP)
    {
        toApMode(this);
    }
}

static void toApMode(const c_esp8266 *this)
{
    m_esp *m_this = {0};
    m_this = this->this;

    switch (m_this->connCode)
    {
        case 0:
            my_uart.send(m_this->uart_num,(u8*)EXIT_SEND_MESSAGE,strlen(EXIT_SEND_MESSAGE));
            m_this->connCode++;
            break;
        case 1:
            verifyAP(this,65535,VERIFY_ADD);
            break;
        case 2:
            my_uart.send(m_this->uart_num,(u8*)CLOSE_ATE,strlen(CLOSE_ATE));
            m_this->connCode++;
            break;
        case 3:
            verifyAP(this,65535,VERIFY_SUB);
            break;
        case 4:
            my_uart.send(m_this->uart_num,(u8*)ATRST,strlen(ATRST));
            m_this->connCode++;
            break;
        case 5:
            verifyAP(this,65535*10,VERIFY_ADD);
            break;
        case 6:
            my_uart.send(m_this->uart_num,(u8*)CWMODE_AP,strlen(CWMODE_AP));
            m_this->connCode++;
            break;
        case 7:
            verifyAP(this,65535,VERIFY_BACK_ZERO);
            break;
        case 8:
            memset(sendBuff,0,sizeof(sendBuff));
            sprintf(sendBuff,CWSAP_SET,m_this->username,m_this->passwd);
            my_uart.send(m_this->uart_num,(u8*)sendBuff,strlen(sendBuff));
            m_this->connCode++;
            break;
        case 9:
            verifyAP(this,65535,VERIFY_BACK_ZERO);
            break;
        case 10:
            my_uart.send(m_this->uart_num,(u8*)CIPAP_SET,strlen(CIPAP_SET));
            m_this->connCode++;
            break;
        case 11:
            verifyAP(this,65535*10,VERIFY_BACK_ZERO);
            break;
        case 12:
            my_uart.send(m_this->uart_num,(u8*)CIPMUX_SET,strlen(CIPMUX_SET));
            m_this->connCode++;
            break;
        case 13:
            verifyAP(this,65535,VERIFY_BACK_ZERO);
            break;
        case 14:
            my_uart.send(m_this->uart_num,(u8*)CIPSERVER_SET,strlen(CIPSERVER_SET));
            m_this->connCode++;
            break;
        case 15:
            verifyAP(this,65535,VERIFY_BACK_ZERO);
            break;
        default:
            getClientMessage(this);
            break;
    }
}

static void verifyAP(const c_esp8266 *this,int cnt,u8 verifyMode)
{
    m_esp *m_this = {0};
    m_this = this->this;

    m_this->connTimeCnt++;
    if(m_this->connTimeCnt>cnt)
    {
        m_this->connTimeCnt=0;
        
        if(verifyMode == VERIFY_ADD)
        {
            m_this->connCode++;
        }
        else if(verifyMode == VERIFY_SUB)
        {
            if(recvFlag)
            {
                if(strstr(recvBuff,"OK\r\n"))
                {
                    m_this->connCode++;
                }
                else if(strstr(recvBuff,"ERROR\r\n"))
                {
                    m_this->connCode--;
                }
                recvFlag=0;
            }
        }
        else if(verifyMode == VERIFY_BACK_ZERO)
        {
            if(recvFlag)
            {
                if(strstr(recvBuff,"OK\r\n"))
                {
                    m_this->connCode++;
                }
                else if(strstr(recvBuff,"ERROR\r\n"))
                {
                    m_this->connCode = 0;
                }
                recvFlag=0;
            }
        }
    }
}

static void getClientMessage(const c_esp8266 *this)
{
    m_esp *m_this = {0};
    m_this = this->this;

    if(recvFlag == 1)
    {
        if(strstr(recvBuff,"0,CONNECT"))
        {
            m_this->clientState = 1;
        }
        else if(strstr(recvBuff,"0,CLOSED"))
        {
            m_this->clientState = 0;
        }
        else if(strstr(recvBuff,"+IPD"))
        {
            char *p = strstr(recvBuff,"+IPD,0");
            if(p!=NULL)
            {
                int dataLen = 0;
                char recvData[32];
                sscanf(p,"+IPD,0,%d:%[^\"]\"",&dataLen,recvData);
                serverCallback((u8*)recvData,dataLen);
            }
        }
        recvFlag = 0;
    }

    if(m_this->sendState == 1) //服务器发送数据
    {
        m_this->connTimeCnt++;
        if(m_this->connTimeCnt > 65535)
        {
            m_this->connTimeCnt=0;
            my_uart.send(m_this->uart_num,(u8*)sendBuff,strlen(sendBuff));
            m_this->sendState=0;
        }
    }
}
