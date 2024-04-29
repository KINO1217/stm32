#include "ds1302.h"
#include <string.h>
#include <stdlib.h>

#define READ_SEC 0x81
#define WRITE_SEC 0x80
#define READ_MIN 0x83
#define WRITE_MIN 0x82
#define READ_HOUR 0x85
#define WRITE_HOUR 0x84
#define READ_DAY 0x87
#define WRITE_DAY 0x86
#define READ_MON 0x89
#define WRITE_MON 0x88
#define READ_WEEK 0x8B
#define WRITE_WEEK 0x8A
#define READ_YEAR 0x8D
#define WRITE_YEAR 0x8C
#define ENABLE_WRITE 0x00
#define DISABLE_WRITE 0x80

typedef struct __MDS1302
{
    GPIO_TypeDef *CLK_GPIOx;
    uint16_t CLK_Pin;
    GPIO_TypeDef *DAT_GPIOx;
    uint16_t DAT_Pin;
    GPIO_TypeDef *RST_GPIOx;
    uint16_t RST_Pin;
} m_ds1302;

enum __LINEMODE
{
    INPUT_MODE = 0,
    OUTPUT_MODE
};

void vDatLineMode(c_ds1302 *this, u8 mode);
void writeRam(c_ds1302 *this, u8 addr, u8 data);
u8 readRam(c_ds1302 *this, u8 addr);
void setTime(c_ds1302 *this, ds_time *time);
void getTime(c_ds1302 *this, ds_time *time);

c_ds1302 ds1302_create(GPIO_TypeDef *CLK_GPIOx, uint16_t CLK_Pin, GPIO_TypeDef *DAT_GPIOx, uint16_t DAT_Pin, GPIO_TypeDef *RST_GPIOx, uint16_t RST_Pin)
{
    // 1.初始化GPIO口（RST和CLK）
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = CLK_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CLK_GPIOx, &GPIO_InitStruct);
    GPIO_ResetBits(CLK_GPIOx, CLK_Pin);

    GPIO_InitStruct.GPIO_Pin = RST_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RST_GPIOx, &GPIO_InitStruct);
    GPIO_ResetBits(RST_GPIOx, RST_Pin);

    // 保存端口变量
    c_ds1302 new = {0};
    new.this = malloc(sizeof(m_ds1302));
    memset(new.this, 0, sizeof(m_ds1302));
    ((m_ds1302 *)new.this)->CLK_GPIOx = CLK_GPIOx;
    ((m_ds1302 *)new.this)->CLK_Pin = CLK_Pin;
    ((m_ds1302 *)new.this)->DAT_GPIOx = DAT_GPIOx;
    ((m_ds1302 *)new.this)->DAT_Pin = DAT_Pin;
    ((m_ds1302 *)new.this)->RST_GPIOx = RST_GPIOx;
    ((m_ds1302 *)new.this)->RST_Pin = RST_Pin;
    new.setTime = setTime;
    new.getTime = getTime;

    return new;
}

static void vDatLineMode(c_ds1302 *this, u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    m_ds1302 *m_this = {0};
    m_this = this->this;

    if (mode == INPUT_MODE)
    {
        GPIO_InitStruct.GPIO_Pin = m_this->DAT_Pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(m_this->DAT_GPIOx, &GPIO_InitStruct);
    }
    else if (mode == OUTPUT_MODE)
    {
        GPIO_InitStruct.GPIO_Pin = m_this->DAT_Pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(m_this->DAT_GPIOx, &GPIO_InitStruct);
    }
}

void writeRam(c_ds1302 *this, u8 addr, u8 data)
{
    m_ds1302 *m_this = {0};
    m_this = this->this;
    vDatLineMode(this, OUTPUT_MODE);

    // SCLK上升沿写入数据，下降沿变换数据
    GPIO_SetBits(m_this->RST_GPIOx, m_this->RST_Pin); // 使能线高电平，开始传输数据

    for (u8 i = 0; i < 8; i++) // 写入命令
    {
        GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 低电平准备数据
        if (addr & 0x01)
        {
            GPIO_SetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        else
        {
            GPIO_ResetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        GPIO_SetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 高电平发送数据
        addr >>= 1;
    }

    for (u8 i = 0; i < 8; i++) // 写入数据
    {
        GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 低电平准备数据
        if (data & 0x01)
        {
            GPIO_SetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        else
        {
            GPIO_ResetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        GPIO_SetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 高电平发送数据
        data >>= 1;
    }

    // 数据写入完成，释放线，停止传输
    GPIO_ResetBits(m_this->RST_GPIOx, m_this->RST_Pin);
    GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin);
}

u8 readRam(c_ds1302 *this, u8 addr)
{
    m_ds1302 *m_this = {0};
    m_this = this->this;
    u8 retDat = 0x00;
    vDatLineMode(this, OUTPUT_MODE);

    // SCLK上升沿写入数据，下降沿变换数据
    GPIO_SetBits(m_this->RST_GPIOx, m_this->RST_Pin); // 使能线高电平，开始传输数据

    for (u8 i = 0; i < 8; i++) // 写入命令
    {
        GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 低电平准备数据
        if (addr & 0x01)
        {
            GPIO_SetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        else
        {
            GPIO_ResetBits(m_this->DAT_GPIOx, m_this->DAT_Pin);
        }
        GPIO_SetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 高电平发送数据
        addr >>= 1;
    }

    vDatLineMode(this, INPUT_MODE);

    for (u8 i = 0; i < 8; i++) // 读取数据
    {
        retDat >>= 1;                                       // 第一位数据是一些状态控制位，可舍弃不读为0
        GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 低电平开始读取数据
        if (GPIO_ReadInputDataBit(m_this->DAT_GPIOx, m_this->DAT_Pin))
        {
            retDat |= 0x80; // 低位先读
        }
        GPIO_SetBits(m_this->CLK_GPIOx, m_this->CLK_Pin); // 切换高电平，准备下一个下降沿读取数据
    }

    GPIO_ResetBits(m_this->RST_GPIOx, m_this->RST_Pin);
    GPIO_ResetBits(m_this->CLK_GPIOx, m_this->CLK_Pin);

    return retDat;
}

void setTime(c_ds1302 *this, ds_time *time)
{
    writeRam(this, 0x8E, ENABLE_WRITE); // 关闭写保护
    time->year = time->year - 2000;
    writeRam(this, WRITE_YEAR, ((time->year / 10 * 16) + (time->year % 10)));
    writeRam(this, WRITE_MON, ((time->mon / 10 * 16) + (time->mon % 10)));
    writeRam(this, WRITE_DAY, ((time->day / 10 * 16) + (time->day % 10)));
    writeRam(this, WRITE_HOUR, ((time->hour / 10 * 16) + (time->hour % 10)));
    writeRam(this, WRITE_MIN, ((time->min / 10 * 16) + (time->min % 10)));
    writeRam(this, WRITE_SEC, ((time->sec / 10 * 16) + (time->sec % 10)));
    writeRam(this, WRITE_WEEK, ((time->week / 10 * 16) + (time->week % 10)));
    writeRam(this, 0x8E, DISABLE_WRITE); // 开启写保护
}

void getTime(c_ds1302 *this, ds_time *time)
{
    u8 tempDat = 0x00;

    tempDat = readRam(this, READ_YEAR);
    time->year = (tempDat / 16 * 10) + (tempDat % 16) + 2000;
    tempDat = readRam(this, READ_MON);
    time->mon = (tempDat / 16 * 10) + (tempDat % 16);
    tempDat = readRam(this, READ_DAY);
    time->day = (tempDat / 16 * 10) + (tempDat % 16);
    tempDat = readRam(this, READ_HOUR);
    time->hour = (tempDat / 16 * 10) + (tempDat % 16);
    tempDat = readRam(this, READ_MIN);
    time->min = (tempDat / 16 * 10) + (tempDat % 16);
    tempDat = readRam(this, READ_SEC);
    time->sec = (tempDat / 16 * 10) + (tempDat % 16);
    tempDat = readRam(this, READ_WEEK);
    time->week = (tempDat / 16 * 10) + (tempDat % 16);
}
