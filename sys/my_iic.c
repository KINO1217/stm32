#include "my_iic.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>

typedef struct __MIIC{
    GPIO_TypeDef* SDA_GPIOx;
    uint16_t SDA_Pin;
    GPIO_TypeDef* SCL_GPIOx;
    uint16_t SCL_Pin;
}m_iic;

static void sdaLineMode(const c_my_iic *this,u8 mode);
static void start(const c_my_iic *this);
static void stop(const c_my_iic *this);
static u8 checkAck(const c_my_iic *this);
static void sendByteData(const c_my_iic *this,u8 data);
c_my_iic iic_create(GPIO_TypeDef* SDA_GPIOx,uint16_t SDA_Pin,GPIO_TypeDef* SCL_GPIOx,uint16_t SCL_Pin)
{
    c_my_iic new = {0};

    // 初始化GPIO口
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = SDA_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//高速模式
    GPIO_Init(SDA_GPIOx,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = SCL_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//高速模式
    GPIO_Init(SCL_GPIOx,&GPIO_InitStruct);

    // 设置默认值
    GPIO_SetBits(SDA_GPIOx,SDA_Pin);
    GPIO_SetBits(SDA_GPIOx,SDA_Pin);

    // 设置c_my_iic结构体变量
    new.this = malloc(sizeof(m_iic));
    memset(new.this,0,sizeof(m_iic));
    ((m_iic*)new.this)->SDA_GPIOx = SDA_GPIOx;
    ((m_iic*)new.this)->SDA_Pin = SDA_Pin;
    ((m_iic*)new.this)->SCL_GPIOx = SCL_GPIOx;
    ((m_iic*)new.this)->SCL_Pin = SCL_Pin;
    new.sdaLineMode = sdaLineMode;
    new.start = start;
    new.stop = stop;
    new.sendByteData = sendByteData;
    new.checkAck = checkAck;

    return new;
}

static void sdaLineMode(const c_my_iic *this,u8 mode)
{
    m_iic *m_this = NULL;
    m_this = this->this;
    GPIO_InitTypeDef GPIO_InitStruct;

    switch(mode)
    {
        case GPIO_MODE_INPUT:
            GPIO_InitStruct.GPIO_Pin = m_this->SDA_Pin;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//上拉输入模式
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//高速模式
            GPIO_Init(m_this->SDA_GPIOx,&GPIO_InitStruct);
            break;
        case GPIO_MODE_OUTPUT:
            GPIO_InitStruct.GPIO_Pin = m_this->SDA_Pin;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//高速模式
            GPIO_Init(m_this->SDA_GPIOx,&GPIO_InitStruct);
            break;
    }
}

static void start(const c_my_iic *this)
{
    this->sdaLineMode(this,GPIO_MODE_OUTPUT); //SDA线为输出模式
    
    m_iic *m_this = NULL;
    m_this = this->this;

    GPIO_SetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    GPIO_SetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
    vDelayUs(5);
    GPIO_ResetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
    vDelayUs(5);
}

static void stop(const c_my_iic *this)
{
    this->sdaLineMode(this,GPIO_MODE_OUTPUT); //SDA线为输出模式
    
    m_iic *m_this = NULL;
    m_this = this->this;

    GPIO_SetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    GPIO_ResetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
    vDelayUs(5);
    GPIO_SetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
    vDelayUs(5);
}

static u8 checkAck(const c_my_iic *this)
{
    u8 ack = 0;
    this->sdaLineMode(this,GPIO_MODE_INPUT); //SDA线为输入模式
    
    m_iic *m_this = NULL;
    m_this = this->this;

    GPIO_ResetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    vDelayUs(3);
    GPIO_SetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    vDelayUs(3);
    if(GPIO_ReadInputDataBit(m_this->SDA_GPIOx,m_this->SDA_Pin))
        ack = 1;
    else
        ack = 0;
    vDelayUs(3);
    GPIO_ResetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    return ack;
}

static void sendByteData(const c_my_iic *this,u8 data)
{
    u8 i = 0;
    this->sdaLineMode(this,GPIO_MODE_OUTPUT); //SDA线为输出模式

    m_iic *m_this = NULL;
    m_this = this->this;

    for(i=0;i<8;i++)
    {
        GPIO_ResetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
        vDelayUs(5);
        if(data&0x80)
            GPIO_SetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
        else
            GPIO_ResetBits(m_this->SDA_GPIOx,m_this->SDA_Pin);
        GPIO_SetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
        vDelayUs(5);
        data <<= 1;
    }
    GPIO_ResetBits(m_this->SCL_GPIOx,m_this->SCL_Pin);
    vDelayUs(5);
}
