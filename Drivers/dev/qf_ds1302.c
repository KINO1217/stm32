#include "qf_ds1302.h"
#include "qf_delay.h"

static void QF_DS1302_WriteRam(uint8_t addr, uint8_t data);
static u8 QF_DS1302_ReadRam(uint8_t addr);
static void QF_DS1302_DATALINEMODE(uint8_t mode);

void QF_DS1302_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = CLK_PIN;
    GPIO_Init(CLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = DATA_PIN;
    GPIO_Init(DATA_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = RST_PIN;
    GPIO_Init(RST_PORT, &GPIO_InitStructure);

    RST_OUT = 0;
    CLK_OUT = 0;
    DATA_OUT = 1;
    QF_DELAY_Us(5);
}

static void QF_DS1302_DATALINEMODE(uint8_t mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    if (mode == 0)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    else if (mode == 1)
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = DATA_PIN;
    GPIO_Init(DATA_PORT, &GPIO_InitStructure);
}

static void QF_DS1302_WriteRam(uint8_t addr, uint8_t data)
{
    QF_DS1302_DATALINEMODE(0);

    RST_OUT = 1;
    QF_DELAY_Us(5);

    for (u8 i = 0; i < 8; i++) // 传输地址命令字节
    {
        CLK_OUT = 0;
        QF_DELAY_Us(5);
        if (addr & 0x01)
            DATA_OUT = 1;
        else
            DATA_OUT = 0;
        QF_DELAY_Us(5);
        CLK_OUT = 1;
        QF_DELAY_Us(5);
        addr >>= 1;
    }

    for (u8 i = 0; i < 8; i++) // 传输数据字节
    {
        CLK_OUT = 0;
        QF_DELAY_Us(5);
        if (data & 0x01)
            DATA_OUT = 1;
        else
            DATA_OUT = 0;
        QF_DELAY_Us(5);
        CLK_OUT = 1;
        QF_DELAY_Us(5);
        data >>= 1;
    }

    RST_OUT = 0;
    CLK_OUT = 0;
    QF_DELAY_Us(5);
}

static u8 QF_DS1302_ReadRam(uint8_t addr)
{
    QF_DS1302_DATALINEMODE(0);

    RST_OUT = 1;
    QF_DELAY_Us(5);

    u8 retData = 0x00;

    for (u8 i = 0; i < 8; i++) {
        CLK_OUT = 0;
        QF_DELAY_Us(5);
        if (addr & 0x01)
            DATA_OUT = 1;
        else
            DATA_OUT = 0;
        QF_DELAY_Us(5);
        CLK_OUT = 1;
        QF_DELAY_Us(5);
        addr >>= 1;
    }

    QF_DS1302_DATALINEMODE(1);

    for (u8 i = 0; i < 8; i++) {
        retData >>= 1;
        CLK_OUT = 0;
        QF_DELAY_Us(5);
        if (DATA_IN)
            retData |= 0x80;
        QF_DELAY_Us(5);
        CLK_OUT = 1;
        QF_DELAY_Us(5);
    }

    RST_OUT = 0;
    CLK_OUT = 0;
    QF_DELAY_Us(5);

    return (retData & 0x7F);
}

void QF_DS1302_SetTime(DS1302_TimeStruct* sys_time)
{
    QF_DS1302_WriteRam(0x8E, ENABLE_WRITE);
    sys_time->year = sys_time->year - 2000;
    QF_DS1302_WriteRam(WRITE_YEAR, ((sys_time->year / 10 * 16) + (sys_time->year % 10)));
    QF_DS1302_WriteRam(WRITE_MONTH, ((sys_time->month / 10 * 16) + (sys_time->month % 10)));
    QF_DS1302_WriteRam(WRITE_DAY, ((sys_time->day / 10 * 16) + (sys_time->day % 10)));
    QF_DS1302_WriteRam(WRITE_WEEKDAY, ((sys_time->week / 10 * 16) + (sys_time->week % 10)));
    QF_DS1302_WriteRam(WRITE_HOUR, ((sys_time->hour / 10 * 16) + (sys_time->hour % 10)));
    QF_DS1302_WriteRam(WRITE_MIN, ((sys_time->min / 10 * 16) + (sys_time->min % 10)));
    QF_DS1302_WriteRam(WRITE_SEC, ((sys_time->sec / 10 * 16) + (sys_time->sec % 10)));
    QF_DS1302_WriteRam(0x8E, DISABLE_WRITE);
}

void QF_DS1302_GetTime(DS1302_TimeStruct* sys_time)
{
    u8 tempData = 0x00;

    tempData = QF_DS1302_ReadRam(READ_YEAR);
    sys_time->year = (tempData / 16 * 10) + (tempData % 16) + 2000;
    tempData = QF_DS1302_ReadRam(READ_MONTH);
    sys_time->month = (tempData / 16 * 10) + (tempData % 16);
    tempData = QF_DS1302_ReadRam(READ_DAY);
    sys_time->day = (tempData / 16 * 10) + (tempData % 16);
    tempData = QF_DS1302_ReadRam(READ_WEEKDAY);
    sys_time->week = (tempData / 16 * 10) + (tempData % 16);
    tempData = QF_DS1302_ReadRam(READ_HOUR);
    sys_time->hour = (tempData / 16 * 10) + (tempData % 16);
    tempData = QF_DS1302_ReadRam(READ_MIN);
    sys_time->min = (tempData / 16 * 10) + (tempData % 16);
    tempData = QF_DS1302_ReadRam(READ_SEC);
    sys_time->sec = (tempData / 16 * 10) + (tempData % 16);
}
