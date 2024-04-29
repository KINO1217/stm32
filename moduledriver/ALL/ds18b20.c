#include "ds18b20.h"
#include "delay.h"
#include "string.h"
#include "stdlib.h"

typedef struct __MDQLINE
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
}m_ds18b20;

enum __DQLINE_MODE
{
    DQLINE_MODE_INPUT=0,
    DQLINE_MODE_OUTPUT,
};

static void vDQLineMode(const c_ds18b20 *this,u8 mode);
static u8 pDS18B20Reset(const c_ds18b20 *this);
static void vDS18B20WriteByte(const c_ds18b20 *this,u8 data);
static u8 pDS18B20ReadByte(const c_ds18b20 *this);
static float fDS18B20GetTemp(const c_ds18b20 *this);

c_ds18b20 ds18b20_create(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    c_ds18b20 new = {0};

    // 初始化GPIO口为输出模式
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx,&GPIO_InitStruct);

    new.this = malloc(sizeof(m_ds18b20));
    memset(new.this,0,sizeof(m_ds18b20));
    ((m_ds18b20*)(new.this))->GPIO_Pin = GPIO_Pin;
    ((m_ds18b20*)(new.this))->GPIOx = GPIOx;
    new.getTemp = fDS18B20GetTemp;

    return new;
}

/**
 * 设置信号线模式
*/
static void vDQLineMode(const c_ds18b20 *this,u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    m_ds18b20 *m_this = {0};
    m_this = this->this;

    switch(mode)
    {
        case DQLINE_MODE_INPUT:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  //上拉输入
            GPIO_InitStruct.GPIO_Pin = m_this->GPIO_Pin;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(m_this->GPIOx,&GPIO_InitStruct);
            break;
        case DQLINE_MODE_OUTPUT:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStruct.GPIO_Pin = m_this->GPIO_Pin;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(m_this->GPIOx,&GPIO_InitStruct);
            break;
    }
}

/**
 * 复位初始化
*/
static u8 pDS18B20Reset(const c_ds18b20 *this)
{
    u8 ack = 1;
    m_ds18b20 *m_this = {0};
    m_this = this->this;

    vDQLineMode(this,DQLINE_MODE_OUTPUT);
    GPIO_ResetBits(m_this->GPIOx,m_this->GPIO_Pin);
    vDelayUs(500);
    GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
    vDelayUs(20);
    vDelayUs(40);
    vDQLineMode(this,DQLINE_MODE_INPUT);
    vDelayUs(40);
    if(GPIO_ReadInputDataBit(m_this->GPIOx,m_this->GPIO_Pin))
        ack=1;
    else
        ack=0;
    vDelayUs(140);
    vDelayUs(300);

    return ack;
}

/**
 * 写入一个字节函数
*/
static void vDS18B20WriteByte(const c_ds18b20 *this,u8 data)
{
    m_ds18b20 *m_this = {0};
    m_this = this->this;

    vDQLineMode(this,DQLINE_MODE_OUTPUT);
    for(u8 i=0;i<8;i++)
    {
        GPIO_ResetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(5);
        if(data&0x01)
            GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
        else
            GPIO_ResetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(10);
        vDelayUs(45);
        GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(3);
        data>>=1;
    }
}

/**
 * 读取一个字节函数
*/
static u8 pDS18B20ReadByte(const c_ds18b20 *this)
{
    m_ds18b20 *m_this = {0};
    m_this = this->this;
    u8 data = 0;

    for(u8 i=0;i<8;i++)
    {
        data>>=1;
        vDQLineMode(this,DQLINE_MODE_OUTPUT);
        GPIO_ResetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(2);
        GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(5);
        vDQLineMode(this,DQLINE_MODE_INPUT);
        if(GPIO_ReadInputDataBit(m_this->GPIOx,m_this->GPIO_Pin))
            data|=0x80;
        vDelayUs(60);
        vDQLineMode(this,DQLINE_MODE_OUTPUT);
        GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
        vDelayUs(2);
    }

    return data;
}

static float fDS18B20GetTemp(const c_ds18b20 *this)
{
    u8 TL = 0;
    u8 TH = 0;
    float temp = 0.0;

    pDS18B20Reset(this);
    vDS18B20WriteByte(this,0xCC);
    vDS18B20WriteByte(this,0x44);
    vDelayMs(100);
    pDS18B20Reset(this);
    vDS18B20WriteByte(this,0xCC);
    vDS18B20WriteByte(this,0xBE);

    TL = pDS18B20ReadByte(this);
    TH = pDS18B20ReadByte(this);

    temp = ((TH<<8)|TL)*0.0625;

    return temp;
}
