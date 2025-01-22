#include "qf_dht11.h"

void QF_DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = DAT_PIN;
    GPIO_Init(DAT_PORT, &GPIO_InitStructure);
}

u8 QF_DHT11_GetData(u8* temp, u8* humi)
{
    uint8_t crc = 1;
    u8 tempData[5] = { 0 };
    int timeout = 0;

    QF_DHT11_Start();
    if (!DAT_IN) {
        while (!DAT_IN && timeout < 80) {
            QF_DELAY_Us(1);
            timeout++;
        }
        timeout = 0;
        while (DAT_IN && timeout < 80) {
            QF_DELAY_Us(1);
            timeout++;
        }

        for (u8 i = 0; i < 5; i++) {
            tempData[i] = QF_DHT11_GetByte();
        }

        *temp = tempData[2];
        *humi = tempData[0];

        for (u8 i = 0; i < 4; i++) {
            crc += tempData[i];
        }
    }

    QF_DATLINEMODE(0);
    DAT_OUT = 1;

    if (crc == tempData[4])
        return 0;
    else
        return 1;
}

void QF_DATLINEMODE(u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if (mode == 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    else if (mode == 1)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = DAT_PIN;
    GPIO_Init(DAT_PORT, &GPIO_InitStructure);
}

void QF_DHT11_Start(void)
{
    QF_DATLINEMODE(0);

    DAT_OUT = 0;
    QF_DELAY_Ms(20);
    DAT_OUT = 1;
    QF_DELAY_Us(40);

    QF_DATLINEMODE(1);
}

u8 QF_DHT11_GetByte(void)
{
    uint8_t dat = 0;
    int timeout = 0;

    QF_DATLINEMODE(1);

    for (u8 i = 0; i < 8; i++) {
        dat <<= 1;
        timeout = 0;
        while (!DAT_IN && timeout < 50) {
            QF_DELAY_Us(1);
            timeout++;
        }
        QF_DELAY_Us(35);
        if (DAT_IN)
            dat |= 0x01;
        timeout = 0;
        while (DAT_IN && timeout < 70) {
            QF_DELAY_Us(1);
            timeout++;
        }
    }

    return dat;
}
