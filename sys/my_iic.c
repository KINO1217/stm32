#include "my_iic.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>

/**
 * IIC引脚配置结构体对象
 */
typedef struct __MIIC
{
    GPIO_TypeDef *sclPort;
    uint16_t sclPin;
    GPIO_TypeDef *sdaPort;
    uint16_t sdaPin;
} m_iic;

static void start(const c_my_iic *this);
static void stop(const c_my_iic *this);
static void sendByteData(const c_my_iic *this,u8 data);
static u8 recvByteData(const c_my_iic *this);
static u8 checkAck(const c_my_iic *this);
static void sendAck(const c_my_iic *this,u8 ackBit);

c_my_iic iic_create(GPIO_TypeDef *sclPort, uint16_t sclPin, GPIO_TypeDef *sdaPort, uint16_t sdaPin)
{
    c_my_iic new = {0};

    /**初始化SDA和SCL引脚端口*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = sclPin;
    // 设置为开漏输出模式，为弱上拉，可输出低电平，
    // 高电平由上拉电阻提供，输出高电平为释放总线，可读取电平状态
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sclPort, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = sdaPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(sdaPort, &GPIO_InitStructure);

    /**设置默认电平*/
    GPIO_SetBits(sdaPort, sdaPin);
    GPIO_SetBits(sclPort, sclPin);

    /**为c_my_iic结构体对象赋值*/
    new.this = malloc(sizeof(m_iic));
    memset(new.this, 0, sizeof(m_iic));
    ((m_iic *)new.this)->sdaPort = sdaPort;
    ((m_iic *)new.this)->sdaPin = sdaPin;
    ((m_iic *)new.this)->sclPort = sclPort;
    ((m_iic *)new.this)->sclPin = sclPin;
    new.start = start;
    new.stop = stop;
    new.sendByteData = sendByteData;
    new.recvByteData = recvByteData;
    new.checkAck = checkAck;
    new.sendAck = sendAck;

    return new;
}

static void start(const c_my_iic *this)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;

    GPIO_SetBits(m_this->sdaPort, m_this->sdaPin);
    GPIO_SetBits(m_this->sclPort, m_this->sclPin);
    vDelayUs(5);
    GPIO_ResetBits(m_this->sdaPort, m_this->sdaPin);
    vDelayUs(5);
    GPIO_ResetBits(m_this->sclPort, m_this->sclPin);
    vDelayUs(5);
}

static void stop(const c_my_iic *this)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;

    GPIO_ResetBits(m_this->sdaPort, m_this->sdaPin);
    GPIO_SetBits(m_this->sclPort, m_this->sclPin);
    vDelayUs(5);
    GPIO_SetBits(m_this->sdaPort, m_this->sdaPin);
    vDelayUs(5);
}

static void sendByteData(const c_my_iic *this,u8 data)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;

    for(u8 i=0;i<8;i++)
    {
        if(data&0x80)
            GPIO_SetBits(m_this->sdaPort,m_this->sdaPin);
        else
            GPIO_ResetBits(m_this->sdaPort,m_this->sdaPin);
        vDelayUs(5);
        data <<= 1;
        GPIO_SetBits(m_this->sclPort,m_this->sclPin);
        vDelayUs(5);
        GPIO_ResetBits(m_this->sclPort,m_this->sclPin);
        vDelayUs(5);
    }
}

static u8 recvByteData(const c_my_iic *this)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;
    u8 recvData = 0x00;

    /**释放SDA线，处于接收模式*/
    GPIO_SetBits(m_this->sdaPort,m_this->sdaPin);
    vDelayUs(5);

    /**接收数据*/
    for(u8 i=0;i<8;i++)
    {
        recvData <<= 1;
        GPIO_SetBits(m_this->sclPort,m_this->sclPin);
        vDelayUs(5);
        if(GPIO_ReadInputDataBit(m_this->sdaPort,m_this->sdaPin))
            recvData |= 0x01;
        GPIO_ResetBits(m_this->sclPort,m_this->sclPin);
        vDelayUs(5);
    }

    return recvData;
}

static u8 checkAck(const c_my_iic *this)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;
    u8 ack = 0x00;

    /**释放SDA线，处于接收模式*/
    GPIO_SetBits(m_this->sdaPort,m_this->sdaPin);
    vDelayUs(5);

    /**接收数据*/
    GPIO_SetBits(m_this->sclPort,m_this->sclPin);
    vDelayUs(5);
    ack = GPIO_ReadInputDataBit(m_this->sdaPort,m_this->sdaPin);
    GPIO_ResetBits(m_this->sclPort,m_this->sclPin);
    vDelayUs(5);

    return ack;
}

static void sendAck(const c_my_iic *this,u8 ackBit)
{
    m_iic *m_this = {0};
    m_this = (m_iic *)this->this;

    if(ackBit == 1)
        GPIO_SetBits(m_this->sdaPort,m_this->sdaPin);
    else
        GPIO_ResetBits(m_this->sdaPort,m_this->sdaPin);
    vDelayUs(5);
    GPIO_SetBits(m_this->sclPort,m_this->sclPin);
    vDelayUs(5);
    GPIO_ResetBits(m_this->sclPort,m_this->sclPin);
    vDelayUs(5);
}
