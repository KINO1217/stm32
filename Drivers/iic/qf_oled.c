#include "qf_oled.h"
#include "qf_oled_font.h"
#include "stdarg.h"
#include "stdio.h"

void QF_OLED_Init(void)
{
    QF_OLED_Gpio_Init();

    QF_OLED_Write_Com(0xA8, 0);
    QF_OLED_Write_Com(0x3F, 0);
    QF_OLED_Write_Com(0xDA, 0);
    QF_OLED_Write_Com(0x12, 0);
    QF_OLED_Write_Com(0xD3, 0);
    QF_OLED_Write_Com(0x00, 0);
    QF_OLED_Write_Com(0x40, 0);
    QF_OLED_Write_Com(0xA1, 0);
    QF_OLED_Write_Com(0x81, 0);
    QF_OLED_Write_Com(0xFF, 0);
    QF_OLED_Write_Com(0xA4, 0);
    QF_OLED_Write_Com(0xA6, 0);
    QF_OLED_Write_Com(0x8D, 0);
    QF_OLED_Write_Com(0x14, 0);
    QF_OLED_Write_Com(0x20, 0);
    QF_OLED_Write_Com(0x02, 0);
    QF_OLED_Write_Com(0xC8, 0);
    QF_OLED_Write_Com(0xB0, 0);
    QF_OLED_Write_Com(0x00, 0);
    QF_OLED_Write_Com(0x10, 0);
    QF_OLED_Write_Com(0xD9, 0);
    QF_OLED_Write_Com(0x22, 0);
    QF_OLED_Write_Com(0xDB, 0);
    QF_OLED_Write_Com(0x20, 0);
    QF_OLED_Write_Com(0xAF, 0);
}

void QF_OLED_Clear(void)
{
    for (u8 i = 0; i < 8; i++) {
        QF_OLED_Write_Com(0xB0 + i, 0);
        QF_OLED_Write_Com(0x00, 0);
        QF_OLED_Write_Com(0x10, 0);
        for (u8 j = 0; j < 128; j++) {
            QF_OLED_Write_Com(0x00, 1);
        }
    }
}

void QF_OLED_On(void)
{
    QF_OLED_Write_Com(0x8D, 0);
    QF_OLED_Write_Com(0x14, 0);
    QF_OLED_Write_Com(0xAF, 0);
}

void QF_OLED_Off(void)
{
    QF_OLED_Write_Com(0x8D, 0);
    QF_OLED_Write_Com(0x10, 0);
    QF_OLED_Write_Com(0xAE, 0);
}

void QF_OLED_Set_Pos(u8 row, u8 col)
{
    QF_OLED_Write_Com(0xB0 + row, 0);
    QF_OLED_Write_Com(0x0F & col, 0);
    QF_OLED_Write_Com(0x10 + ((0xF0 & col) >> 4), 0);
}

void QF_OLED_Show_Char(u8 row, u8 col, u8 ch)
{
    if (row > 7 || col > 127)
        return;

    int index = ch - ' ';

    for (u8 i = 0; i < 2; i++) {
        QF_OLED_Set_Pos(row + i, col);
        for (u8 j = 0; j < 8; j++) {
            QF_OLED_Write_Com(cFont8X16[index][i * 8 + j], 1);
        }
    }
}

void QF_OLED_Show_String(u8 row, u8 col, u8* str, ...)
{
    u8 tempStr[16];
    u8 len = 0;

    va_list vaList;
    va_start(vaList, str);
    vsprintf((char*)tempStr, (char*)str, vaList);
    va_end(vaList); // 拼接字符串

    while (tempStr[len] != '\0') {
        QF_OLED_Show_Char(row, col, tempStr[len]);
        len++;
        col += 8;
        if (col > 127) {
            row += 2;
            col = col % 128;
        }
    }
}

void QF_OLED_Show_Pic(void)
{
    u8 row = 0;
    u8 col = 0;

    for (row = 0; row < 8; row++) {
        QF_OLED_Set_Pos(row, 31);
        for (col = 0; col < 64; col++) {
            QF_OLED_Write_Com(dog_pic[row * 64 + col], 1);
        }
    }
}

void QF_OLED_Write_Com(u8 data, u8 com)
{
    QF_OLED_IIC_Start();
    QF_OLED_IIC_Send_Byte(0x78);
    QF_OLED_IIC_GetAck();
    if (com == 0)
        QF_OLED_IIC_Send_Byte(0x00);
    else if (com == 1)
        QF_OLED_IIC_Send_Byte(0x40);
    QF_OLED_IIC_GetAck();
    QF_OLED_IIC_Send_Byte(data);
    QF_OLED_IIC_GetAck();
    QF_OLED_IIC_Stop();
}

void QF_DELAY_5US(void)
{
    u8 i = 50;

    while (i--) {
        __NOP();
    }
}

void QF_OLED_IIC_Start(void)
{
    SDA_H;
    SCL_H;
    QF_DELAY_5US();
    SDA_L;
    QF_DELAY_5US();
    SCL_L;
    QF_DELAY_5US();
}

void QF_OLED_IIC_Stop(void)
{
    SDA_L;
    SCL_H;
    QF_DELAY_5US();
    SDA_H;
    QF_DELAY_5US();
}

void QF_OLED_IIC_Send_Byte(u8 data)
{
    u8 i = 0;

    for (i = 0; i < 8; i++) {
        SCL_L;
        QF_DELAY_5US();
        if ((data << i) & 0x80)
            SDA_H;
        else
            SDA_L;
        SCL_H;
        QF_DELAY_5US();
    }

    SCL_L;
    QF_DELAY_5US();
}

u8 QF_OLED_IIC_GetAck(void)
{
    u8 ack = 1;

    SDA_H; // 释放数据总线

    SCL_H; // 拉高时钟线
    QF_DELAY_5US();
    ack = SDA_IN;

    SCL_L; // 拉低时钟线
    QF_DELAY_5US();

    return ack;
}

void QF_OLED_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SCL_PIN;
    GPIO_Init(SCL_GPIO, &GPIO_InitStructure);
}
