#ifndef __DS18B20_H__
#define __DS18B20_H__
#include "stm32f10x.h"

/**
 * @brief ds18b20驱动文件，自写，可随意移植开源
 * @author KINO
 * @date 2024/4/20
*/

typedef struct __MDS18B20 c_ds18b20;
typedef struct __MDS18B20
{
    void *this;
    /**
     * @brief 读取温度
     * @param this ds18b20对象
     * @return float 温度
     * 示例：
        float temp = 0.0;

        temp = ds18b20.getTemp(&ds18b20);
    */
    float (*getTemp)(const c_ds18b20 *this);
}c_ds18b20;

/**
 * @brief ds18b20初始化
 * @param GPIOx GPIO端口
 * @param GPIO_Pin GPIO引脚
 * @return c_ds18b20 ds18b20对象
 * 示例：
    c_ds18b20 ds18b20 = {0};

    ds18b20 = ds18b20_create(GPIOB, GPIO_Pin_0);
*/
c_ds18b20 ds18b20_create(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif
