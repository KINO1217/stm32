#include "qf_oled.h"
#include "qf_oled_font.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* OLED显存：8页 * 128列 */
uint8_t QF_OLED_GRAM[QF_OLED_PAGE_NUM][QF_OLED_WIDTH];

/* GPIO操作宏 */
#define QF_OLED_SDA_H()     GPIO_SetBits(QF_OLED_SDA_PORT, QF_OLED_SDA_PIN)
#define QF_OLED_SDA_L()     GPIO_ResetBits(QF_OLED_SDA_PORT, QF_OLED_SDA_PIN)

#define QF_OLED_SCL_H()     GPIO_SetBits(QF_OLED_SCL_PORT, QF_OLED_SCL_PIN)
#define QF_OLED_SCL_L()     GPIO_ResetBits(QF_OLED_SCL_PORT, QF_OLED_SCL_PIN)

/* 简单延时 */
static void QF_OLED_IIC_Delay(void)
{
    uint8_t i;
    for(i = 0; i < 8; i++);
}

/* GPIO初始化 */
static void QF_OLED_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = QF_OLED_SDA_PIN | QF_OLED_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    QF_OLED_SDA_H();
    QF_OLED_SCL_H();
}

/* 软件IIC起始 */
static void QF_OLED_IIC_Start(void)
{
    QF_OLED_SDA_H();
    QF_OLED_SCL_H();
    QF_OLED_IIC_Delay();
    QF_OLED_SDA_L();
    QF_OLED_IIC_Delay();
    QF_OLED_SCL_L();
}

/* 软件IIC停止 */
static void QF_OLED_IIC_Stop(void)
{
    QF_OLED_SCL_L();
    QF_OLED_SDA_L();
    QF_OLED_IIC_Delay();
    QF_OLED_SCL_H();
    QF_OLED_IIC_Delay();
    QF_OLED_SDA_H();
    QF_OLED_IIC_Delay();
}

/* 软件IIC发送1字节，不检测ACK */
static void QF_OLED_IIC_SendByte(uint8_t byte)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        QF_OLED_SCL_L();

        if(byte & 0x80)
            QF_OLED_SDA_H();
        else
            QF_OLED_SDA_L();

        QF_OLED_IIC_Delay();
        QF_OLED_SCL_H();
        QF_OLED_IIC_Delay();
        QF_OLED_SCL_L();

        byte <<= 1;
    }

    /* 跳过ACK */
    QF_OLED_SDA_H();
    QF_OLED_IIC_Delay();
    QF_OLED_SCL_H();
    QF_OLED_IIC_Delay();
    QF_OLED_SCL_L();
    QF_OLED_IIC_Delay();
}

/* 写命令 */
static void QF_OLED_WriteCmd(uint8_t cmd)
{
    QF_OLED_IIC_Start();
    QF_OLED_IIC_SendByte(QF_OLED_IIC_ADDR);
    QF_OLED_IIC_SendByte(0x00);
    QF_OLED_IIC_SendByte(cmd);
    QF_OLED_IIC_Stop();
}

/* 连续写数据 */
static void QF_OLED_WriteDataBurst(uint8_t *pData, uint16_t len)
{
    uint16_t i;

    QF_OLED_IIC_Start();
    QF_OLED_IIC_SendByte(QF_OLED_IIC_ADDR);
    QF_OLED_IIC_SendByte(0x40);

    for(i = 0; i < len; i++)
    {
        QF_OLED_IIC_SendByte(pData[i]);
    }

    QF_OLED_IIC_Stop();
}

/* 设置位置 */
static void QF_OLED_SetPos(uint8_t x, uint8_t page)
{
    QF_OLED_WriteCmd(0xB0 + page);
    QF_OLED_WriteCmd(0x10 + ((x >> 4) & 0x0F));
    QF_OLED_WriteCmd(0x00 + (x & 0x0F));
}

/* 开显示 */
void QF_OLED_DisplayOn(void)
{
    QF_OLED_WriteCmd(0x8D);
    QF_OLED_WriteCmd(0x14);
    QF_OLED_WriteCmd(0xAF);
}

/* 关显示 */
void QF_OLED_DisplayOff(void)
{
    QF_OLED_WriteCmd(0x8D);
    QF_OLED_WriteCmd(0x10);
    QF_OLED_WriteCmd(0xAE);
}

/* 清缓存 */
void QF_OLED_Clear(void)
{
    uint8_t page, x;

    for(page = 0; page < QF_OLED_PAGE_NUM; page++)
    {
        for(x = 0; x < QF_OLED_WIDTH; x++)
        {
            QF_OLED_GRAM[page][x] = 0x00;
        }
    }
}

/* 填充缓存 */
void QF_OLED_Fill(uint8_t data)
{
    uint8_t page, x;

    for(page = 0; page < QF_OLED_PAGE_NUM; page++)
    {
        for(x = 0; x < QF_OLED_WIDTH; x++)
        {
            QF_OLED_GRAM[page][x] = data;
        }
    }
}

/* 刷新显示 */
void QF_OLED_Refresh(void)
{
    uint8_t page;

    for(page = 0; page < QF_OLED_PAGE_NUM; page++)
    {
        QF_OLED_SetPos(0, page);
        QF_OLED_WriteDataBurst(QF_OLED_GRAM[page], QF_OLED_WIDTH);
    }
}

/* 画点 */
void QF_OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t page;
    uint8_t bit;

    if(x >= QF_OLED_WIDTH || y >= QF_OLED_HEIGHT)
        return;

    page = y / 8;
    bit  = y % 8;

    if(t)
        QF_OLED_GRAM[page][x] |= (1 << bit);
    else
        QF_OLED_GRAM[page][x] &= ~(1 << bit);
}

/* 清除区域 */
void QF_OLED_ClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t i, j;

    for(i = x; i < x + w && i < QF_OLED_WIDTH; i++)
    {
        for(j = y; j < y + h && j < QF_OLED_HEIGHT; j++)
        {
            QF_OLED_DrawPoint(i, j, 0);
        }
    }
}

/* 显示8x16字符 */
void QF_OLED_ShowChar8x16(uint8_t x, uint8_t y, char chr)
{
    uint8_t c;
    uint8_t i, j;
    uint8_t line;

    if(x > QF_OLED_WIDTH - 8 || y > QF_OLED_HEIGHT - 16)
        return;

    if(chr < ' ' || chr > '~')
        chr = '?';

    c = chr - ' ';

    for(i = 0; i < 8; i++)
    {
        line = QF_OLED_F8x16[c][i];
        for(j = 0; j < 8; j++)
        {
            if(line & (1 << j))
                QF_OLED_DrawPoint(x + i, y + j, 1);
            else
                QF_OLED_DrawPoint(x + i, y + j, 0);
        }
    }

    for(i = 0; i < 8; i++)
    {
        line = QF_OLED_F8x16[c][i + 8];
        for(j = 0; j < 8; j++)
        {
            if(line & (1 << j))
                QF_OLED_DrawPoint(x + i, y + 8 + j, 1);
            else
                QF_OLED_DrawPoint(x + i, y + 8 + j, 0);
        }
    }
}

/* 显示8x16字符串 */
void QF_OLED_ShowString8x16(uint8_t x, uint8_t y, const char *str)
{
    while(*str)
    {
        if(x > QF_OLED_WIDTH - 8)
        {
            x = 0;
            y += 16;
        }

        if(y > QF_OLED_HEIGHT - 16)
            break;

        QF_OLED_ShowChar8x16(x, y, *str);
        x += 8;
        str++;
    }
}

/* 幂函数 */
static uint32_t QF_OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)
    {
        result *= m;
    }

    return result;
}

/* 显示十进制数 */
void QF_OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t i;
    uint8_t temp;
    uint8_t enshow = 0;

    for(i = 0; i < len; i++)
    {
        temp = (num / QF_OLED_Pow(10, len - i - 1)) % 10;

        if(enshow == 0 && i < (len - 1))
        {
            if(temp == 0)
            {
                QF_OLED_ShowChar8x16(x + i * 8, y, ' ');
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        QF_OLED_ShowChar8x16(x + i * 8, y, temp + '0');
    }
}

/* 显示十六进制数 */
void QF_OLED_ShowHexNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len)
{
    uint8_t i;
    uint8_t temp;
    char ch;

    for(i = 0; i < len; i++)
    {
        temp = (num >> ((len - 1 - i) * 4)) & 0x0F;

        if(temp < 10)
            ch = temp + '0';
        else
            ch = temp - 10 + 'A';
        QF_OLED_ShowChar8x16(x + i * 8, y, ch);
    }
}

/* Printf 8x16 */
void QF_OLED_Printf8x16(uint8_t x, uint8_t y, const char *format, ...)
{
    char buf[QF_OLED_PRINTF_BUF_LEN];
    va_list args;

    memset(buf, 0, sizeof(buf));

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);

    QF_OLED_ShowString8x16(x, y, buf);
}

/* OLED初始化 */
void QF_OLED_Init(void)
{
    QF_OLED_GPIO_Init();

    QF_OLED_WriteCmd(0xAE);
    QF_OLED_WriteCmd(0xD5);
    QF_OLED_WriteCmd(0x80);
    QF_OLED_WriteCmd(0xA8);
    QF_OLED_WriteCmd(0x3F);
    QF_OLED_WriteCmd(0xD3);
    QF_OLED_WriteCmd(0x00);
    QF_OLED_WriteCmd(0x40);
    QF_OLED_WriteCmd(0x8D);
    QF_OLED_WriteCmd(0x14);
    QF_OLED_WriteCmd(0x20);
    QF_OLED_WriteCmd(0x02);
    QF_OLED_WriteCmd(0xA1);
    QF_OLED_WriteCmd(0xC8);
    QF_OLED_WriteCmd(0xDA);
    QF_OLED_WriteCmd(0x12);
    QF_OLED_WriteCmd(0x81);
    QF_OLED_WriteCmd(0xCF);
    QF_OLED_WriteCmd(0xD9);
    QF_OLED_WriteCmd(0xF1);
    QF_OLED_WriteCmd(0xDB);
    QF_OLED_WriteCmd(0x30);
    QF_OLED_WriteCmd(0xA4);
    QF_OLED_WriteCmd(0xA6);

    QF_OLED_Clear();
    QF_OLED_Refresh();
    QF_OLED_DisplayOn();
}
