#include "jq8400.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>
#include "my_uart.h"

typedef struct __MJQ8400
{
    GPIO_TypeDef *gpioPort;
    uint16_t gpioPin;
    u8 uartCode;
    u8 conMode;
}m_jq8400;

static void vJq8400SendByte(const c_jq8400 *this,u8 cmdOrData);
static void volume(const c_jq8400 *this,u8 vol);
static void play(const c_jq8400 *this,u8 cnt,u8 loop);
static void stop(const c_jq8400 *this,u8 mode);

c_jq8400 jq8400_create(GPIO_TypeDef *gpioPort,uint16_t gpioPin,u8 uartCode,u8 conMode)
{
    c_jq8400 new = {0};

    /**为c_jq8400结构体赋值*/
    new.this = malloc(sizeof(m_jq8400));
    memset(new.this,0,sizeof(m_jq8400));
    ((m_jq8400*)(new.this))->gpioPort = gpioPort;
    ((m_jq8400*)(new.this))->gpioPin = gpioPin;
    ((m_jq8400*)(new.this))->uartCode = uartCode;
    ((m_jq8400*)(new.this))->conMode = conMode;
    new.volume = volume;
    new.play = play;
    new.stop = stop;

    /**根据模式初始化*/
    if(conMode == ONELINE_MODE)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.GPIO_Pin = gpioPin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(gpioPort,&GPIO_InitStruct);
    }
    else if(conMode == UART_MODE)
    {
        my_uart.init(uartCode,9600,ISR_MODE);
    }

    return new;
}

/**
 * 一线协议发送数据函数，音频文件命名格式为5个数字
*/
static void vJq8400SendByte(const c_jq8400 *this,u8 cmdOrData)
{
    m_jq8400 *m_this = {0};
    m_this = (m_jq8400*)(this->this);

    /**发送引导码*/
    GPIO_ResetBits(m_this->gpioPort,m_this->gpioPin);
    vDelayMs(4);

    /**发送数据*/
    for(u8 i=0;i<8;i++)
    {
        GPIO_SetBits(m_this->gpioPort,m_this->gpioPin);
        if(cmdOrData&0x01)
        {
            vDelayUs(1200);
            GPIO_ResetBits(m_this->gpioPort,m_this->gpioPin);
            vDelayUs(400);
        }
        else
        {
            vDelayUs(400);
            GPIO_ResetBits(m_this->gpioPort,m_this->gpioPin);
            vDelayUs(1200);
        }
        cmdOrData>>=1;
    }

    /**恢复空闲状态*/
    GPIO_SetBits(m_this->gpioPort,m_this->gpioPin);
    vDelayMs(5);
}

static void volume(const c_jq8400 *this,u8 vol)
{
    m_jq8400 *m_this = {0};
    m_this = (m_jq8400*)(this->this);

    if(m_this->conMode == ONELINE_MODE)
    {
        vJq8400SendByte(this,0x0A);
        vJq8400SendByte(this,vol/10);
        vJq8400SendByte(this,vol%10);
        vJq8400SendByte(this,0x0C);
    }
    else if (m_this->conMode == UART_MODE)
    {
        u8 cmd[5]={0};

        cmd[0]=0xAA;
        cmd[1]=0x13;
        cmd[2]=0x01;
        cmd[3]=vol;
        cmd[4]=0xAA+0x13+0x01+vol;
        my_uart.send(m_this->uartCode,cmd,5);
    }
}

static void play(const c_jq8400 *this,u8 cnt,u8 loop)
{
    m_jq8400 *m_this = {0};
    m_this = (m_jq8400*)(this->this);

    if(m_this->conMode == ONELINE_MODE)
    {
        /**选曲*/
        vJq8400SendByte(this,0x0A);
        vJq8400SendByte(this,cnt/100);
        vJq8400SendByte(this,cnt/10%10);
        vJq8400SendByte(this,cnt%10);
        vJq8400SendByte(this,0x0B);

        /**设置循环模式*/
        vJq8400SendByte(this,0x0A);
        if(loop==0)
        {
            vJq8400SendByte(this,0x02);
        }
        else if(loop == 1)
        {
            vJq8400SendByte(this,0x01);
        }
        vJq8400SendByte(this,0x0E);

        /**播放选曲*/
        vJq8400SendByte(this,0x11);
    }
    else if(m_this->conMode == UART_MODE)
    {
        u8 cmd[6]={0};

        /**设置循环模式*/
        cmd[0]=0xAA;
        cmd[1]=0x18;
        cmd[2]=0x01;
        if(loop == 0)
        {
            cmd[3]=0x02;
            cmd[4]=0xC5;
        }
        else if(loop == 1)
        {
            cmd[3]=0x01;
            cmd[4]=0xC4;
        }
        my_uart.send(m_this->uartCode,cmd,5);

        /**播放选曲*/
        cmd[0]=0xAA;
        cmd[1]=0x07;
        cmd[2]=0x02;
        cmd[3]=cnt/255;
        cmd[4]=cnt%255;
        cmd[5]=0xAA+0x07+0x02+cnt;
        my_uart.send(m_this->uartCode,cmd,6);
    }
}

static void stop(const c_jq8400 *this,u8 mode)
{
    m_jq8400 *m_this = {0};
    m_this = (m_jq8400*)(this->this);

    if(m_this->conMode == ONELINE_MODE)
    {
        if(mode==0)
        {
            /**停止播放*/
            vJq8400SendByte(this,0x13);
        }
        else if(mode==1)
        {
            /**暂停播放*/
            vJq8400SendByte(this,0x12);
        }
        else if(mode==2)
        {
            /*继续播放*/
            vJq8400SendByte(this,0x11);
        }
    }
    else if (m_this->conMode == UART_MODE)
    {
        u8 cmd[4]={0};

        if(mode==0)
        {
            cmd[0]=0xAA;
            cmd[1]=0x04;
            cmd[2]=0x00;
            cmd[3]=0xAE;
            my_uart.send(m_this->uartCode,cmd,4);
        }
        else if(mode==1)
        {
            cmd[0]=0xAA;
            cmd[1]=0x03;
            cmd[2]=0x00;
            cmd[3]=0xAD;
            my_uart.send(m_this->uartCode,cmd,4);
        }
        else if(mode==2)
        {
            cmd[0]=0xAA;
            cmd[1]=0x02;
            cmd[2]=0x00;
            cmd[3]=0xAC;
            my_uart.send(m_this->uartCode,cmd,4);
        }
    }
}
