#ifndef __QF_DS1302_H__
#define __QF_DS1302_H__

#include "qf_sys.h"
#include "stm32f10x.h"

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int week;
} DS1302_TimeStruct;

// ¼Ä´æÆ÷
#define WRITE_SEC 0x80
#define READ_SEC 0x81
#define WRITE_MIN 0x82
#define READ_MIN 0x83
#define WRITE_HOUR 0x84
#define READ_HOUR 0x85
#define WRITE_DAY 0x86
#define READ_DAY 0x87
#define WRITE_MONTH 0x88
#define READ_MONTH 0x89
#define WRITE_WEEKDAY 0x8A
#define READ_WEEKDAY 0x8B
#define WRITE_YEAR 0x8C
#define READ_YEAR 0x8D
#define ENABLE_WRITE 0x00
#define DISABLE_WRITE 0x80

#define CLK_PORT GPIOB
#define CLK_PIN GPIO_Pin_9
#define DATA_PORT GPIOB
#define DATA_PIN GPIO_Pin_8
#define RST_PORT GPIOB
#define RST_PIN GPIO_Pin_7

#define CLK_OUT PBout(9)
#define DATA_OUT PBout(8)
#define DATA_IN PBin(8)
#define RST_OUT PBout(7)

void QF_DS1302_Init(void);
void QF_DS1302_SetTime(DS1302_TimeStruct* sys_time);
void QF_DS1302_GetTime(DS1302_TimeStruct* sys_time);

#endif
