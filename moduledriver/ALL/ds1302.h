#ifndef __DS1302_H__
#define __DS1302_H__
#include "stm32f10x.h"

/**
 * @brief DS1302时钟模块驱动，使用伪SPI协议
 * @author KINO
 * @date 2024/4/28
*/

typedef struct __DSTIME
{
    int year; //十进制数据，需转换为BCD码
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int week;
}ds_time;

typedef struct __DS1302 c_ds1302;
typedef struct __DS1302
{
    void *this;
    /**
     * @note 设置时间，设置一次即可
     * @param time 时间结构体
     * 示例：
        ds_time time = {2024,4,29,22,38,0,1};
        ds1302.setTime(&ds1302,&time);
    */
    void (*setTime)(c_ds1302 *this,ds_time *time);
    /**
     * @note 获取时间
     * @param time 时间结构体
     * 示例：
        ds_time time = {0};
        ds1302.getTime(&ds1302,&time);
    */
    void (*getTime)(c_ds1302 *this,ds_time *time);
}c_ds1302;

/**
 * @note 创建DS1302对象
 * @param CLK_GPIOx CLK引脚所在GPIO
 * @param CLK_Pin CLK引脚
 * @param DAT_GPIOx DAT引脚所在GPIO
 * @param DAT_Pin DAT引脚
 * @param RST_GPIOx RST引脚所在GPIO
 * @param RST_Pin RST引脚
 * @return DS1302对象
 * 示例：
    c_ds1302 ds1302 = {0};
    ds1302 = ds1302_create(GPIOC,GPIO_Pin_13,GPIOC,GPIO_Pin_14,GPIOC,GPIO_Pin_15);
*/
c_ds1302 ds1302_create(GPIO_TypeDef* CLK_GPIOx,uint16_t CLK_Pin,GPIO_TypeDef* DAT_GPIOx,uint16_t DAT_Pin,GPIO_TypeDef* RST_GPIOx,uint16_t RST_Pin);

#endif
