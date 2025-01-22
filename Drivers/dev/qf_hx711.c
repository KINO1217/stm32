#include "qf_hx711.h"

static void QF_HX711_DATLINEMODE(u8 mode);
static int QF_HX711_ReadData(void);
int zero = 0;
#define gapValue 404.08 // 校准参数

void QF_HX711_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = HX711SCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX711SCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = HX711DAT_PIN;
    GPIO_Init(HX711DAT_PORT, &GPIO_InitStructure);
}

float QF_HX711_GetWeight(void)
{
    float weight = 0.0;
    int tempWeight = 0;

    for (u8 i = 0; i < 7; i++) {
        tempWeight += QF_HX711_ReadData();
    }

    tempWeight /= 7;

    if (tempWeight > zero)
        weight = (float)(tempWeight - zero) / gapValue;
    else
        weight = 0.0;

    return weight;
}

void QF_HX711_GetZero(void)
{
    for (u8 i = 0; i < 7; i++) {
        zero += QF_HX711_ReadData();
    }
    zero /= 7;
}

static int QF_HX711_ReadData(void)
{
    int count = 0;
    int timeout = 0;

    QF_HX711_DATLINEMODE(0);

    HXDAT_OUT = 1;
    HXSCL_OUT = 0; // 默认初始化模式

    QF_HX711_DATLINEMODE(1);

    while (HXDAT_IN && timeout < 100) {
        QF_DELAY_Ms(1);
        timeout++;
    } // 等待数据准备好，大概100ms,10Hz

    for (u8 i = 0; i < 24; i++) {
        HXSCL_OUT = 1; // 上升沿准备数据
        count <<= 1;
        HXSCL_OUT = 0; // 下降沿采集数据
        if (HXDAT_IN)
            count |= 0x01;
    }

    HXSCL_OUT = 1; // 产生25个脉冲信号标识使用A通道128增益模式
    HXSCL_OUT = 0;

    count ^= 0x800000;

    return count;
}

static void QF_HX711_DATLINEMODE(u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = HX711DAT_PIN;
    if (mode == 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    else if (mode == 1)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HX711DAT_PORT, &GPIO_InitStructure);
}
