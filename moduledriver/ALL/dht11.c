#include "dht11.h"
#include "delay.h"
#include "stdlib.h"
#include "string.h"

typedef struct __MSDHT11
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
}m_dht11;

enum __GPIO_MODE
{
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT
};

static u8 dataArray[5];

static void vDataLineMode(const c_dht11 *this,u8 mode);
static void vStart(const c_dht11 *this);
static u8 pxGetByte(const c_dht11 *this);
static u8 get(const c_dht11 *this,u8 *temp,u8 *humi);

c_dht11 dht11_create(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
    c_dht11 new = {0};
    GPIO_InitTypeDef GPIO_InitStructure;

    new.this = malloc(sizeof(m_dht11));
    memset(new.this,0,sizeof(m_dht11));
    ((m_dht11*)(new.this))->GPIOx = GPIOx;
    ((m_dht11*)(new.this))->GPIO_Pin = GPIO_Pin;
    new.get = get;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    GPIO_SetBits(GPIOx, GPIO_Pin);
    vDelayMs(1000);

    return new;
}

//设置GPIO模式，输入/输出
static void vDataLineMode(const c_dht11 *this,u8 mode)
{
    m_dht11 *m_this = {0};
    m_this = this->this;
    GPIO_InitTypeDef GPIO_InitStructure;

    switch (mode)
    {
        case GPIO_MODE_INPUT:
            GPIO_InitStructure.GPIO_Pin = m_this->GPIO_Pin;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(m_this->GPIOx, &GPIO_InitStructure);
            break;
        case GPIO_MODE_OUTPUT:
            GPIO_InitStructure.GPIO_Pin = m_this->GPIO_Pin;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(m_this->GPIOx, &GPIO_InitStructure);
            GPIO_SetBits(m_this->GPIOx, m_this->GPIO_Pin);
            break;
    }
}

/**
 * 起始信号
*/
static void vStart(const c_dht11 *this)
{
    m_dht11 *m_this = {0};
    m_this = this->this;

    vDataLineMode(this,GPIO_MODE_OUTPUT);
    GPIO_ResetBits(m_this->GPIOx, m_this->GPIO_Pin);
    vDelayMs(20);
    GPIO_SetBits(m_this->GPIOx, m_this->GPIO_Pin);
    vDelayUs(40);
    vDataLineMode(this,GPIO_MODE_INPUT);
}

/**
 * 获取一个字节数据
*/
static u8 pxGetByte(const c_dht11 *this)
{
    u8 tempData = 0;
    m_dht11 *m_this = {0};
    m_this = this->this;

    for(u8 i=0;i<8;i++)
    {
        while(!GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin));
        tempData<<=1;
        vDelayUs(40);
        if(GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin))
            tempData |= 0x01;
        while(GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin));
    }

    return tempData;
}

//获取温湿度值
static u8 get(const c_dht11 *this,u8 *temp,u8 *humi)
{
    u8 crc = 0;
    m_dht11 *m_this = {0};
    m_this = this->this;

    vStart(this);
    if(!GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin))
    {
        while(!GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin));
        while(GPIO_ReadInputDataBit(m_this->GPIOx, m_this->GPIO_Pin));

        for(u8 i=0;i<5;i++)
            dataArray[i] = pxGetByte(this);

        *temp = dataArray[2];
        *humi = dataArray[0];

        for(u8 i=0;i<4;i++)
            crc+=dataArray[i];
    }
    vDataLineMode(this,GPIO_MODE_OUTPUT);

    if(crc == dataArray[4])
        return 0;
    else 
        return 1;
}
