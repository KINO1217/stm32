#include "qf_ds18b20.h"

void QF_DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = DQ_PIN;
    GPIO_Init(DQ_PORT, &GPIO_InitStructure);
}

void QF_DS18B20_GetTemp(float* temp)
{
    uint8_t TL, TH;

    QF_DS18B20_Reset();
    QF_DS18B20_WriteByte(0xCC);
    QF_DS18B20_WriteByte(0x44);
    QF_DELAY_Ms(5);
    QF_DS18B20_Reset();
    QF_DS18B20_WriteByte(0xCC);
    QF_DS18B20_WriteByte(0xBE);

    TL = QF_DS18B20_ReadByte();
    TH = QF_DS18B20_ReadByte();

    *temp = (float)(((TH << 8) | TL) * 0.0625);
}

void QF_DS18B20_DQLINEMODE(u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if (mode == 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    else if (mode == 1)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = DQ_PIN;
    GPIO_Init(DQ_PORT, &GPIO_InitStructure);
}

u8 QF_DS18B20_Reset(void)
{
    u8 ack = 1;

    QF_DS18B20_DQLINEMODE(0);
    DQ_OUT = 0; // 拉低总线至少480us
    QF_DELAY_Us(480);

    DQ_OUT = 1; // 释放总线15-60us
    QF_DELAY_Us(30);
    QF_DELAY_Us(30);
    QF_DS18B20_DQLINEMODE(1);
    QF_DELAY_Us(90);
    ack = DQ_IN; // 读取DS18B20存在脉冲，60-240us之间读取
    QF_DELAY_Us(90);
    QF_DELAY_Us(240); // 从释放总线到接收存在脉冲结束需要480us

    return ack;
}

void QF_DS18B20_WriteByte(u8 dat)
{
    QF_DS18B20_DQLINEMODE(0);

    for (uint8_t i = 0; i < 8; i++) {
        DQ_OUT = 0;
        QF_DELAY_Us(5);
        if (dat & 0x01)
            DQ_OUT = 1;
        QF_DELAY_Us(10);
        QF_DELAY_Us(45);
        DQ_OUT = 1;
        QF_DELAY_Us(3);
        dat >>= 1;
    }
}

u8 QF_DS18B20_ReadByte(void)
{
    uint8_t data = 0;

    for (uint8_t i = 0; i < 8; i++) {
        QF_DS18B20_DQLINEMODE(0);
        data >>= 1;
        DQ_OUT = 0; // 至少1us
        QF_DELAY_Us(3);
        DQ_OUT = 1; // 释放总线
        QF_DELAY_Us(6);
        QF_DS18B20_DQLINEMODE(1);
        if (DQ_IN)
            data |= 0x80;
        QF_DELAY_Us(6); // 15us内控制器进行采样
        QF_DELAY_Us(45);
        QF_DS18B20_DQLINEMODE(0);
        DQ_OUT = 1;
        QF_DELAY_Us(3); // 释放总线至少1us，两个数据间隔
    }

    return data;
}
