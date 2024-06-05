#ifndef __MY_ADC_H__
#define __MY_ADC_H__
#include <stm32f10x.h>

/**
 * @brief ADC外设驱动，使用单次非扫描模式进行电压采集
 * @author KINO
 * @copyright KINO All Rights Reserved
*/

enum ADCCHANNAL
{
    MY_ADC_0=0,     //PA0
    MY_ADC_1,       //PA1
    MY_ADC_2,       //PA2
    MY_ADC_3,       //PA3
    MY_ADC_4,       //PA4
    MY_ADC_5,       //PA5
    MY_ADC_6,       //PA6
    MY_ADC_7,       //PA7
    MY_ADC_8,       //PB0
    MY_ADC_9,       //PB1
};

typedef struct __MADC
{
    void (*init)(u8 adcChannal);
    void (*get)(u8 adcChannel,float *v);
}c_my_adc;

/**
 * 全局单例对象
*/
extern const c_my_adc my_adc;

#endif
