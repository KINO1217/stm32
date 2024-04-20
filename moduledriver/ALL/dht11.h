#ifndef __DHT11_H__
#define __DHT11_H__
#include "stm32f10x.h"

/**
 * @brief DHT11 sensor
 * @author KINO
 * @date 2024/4/21
*/

typedef struct __MDHT11 c_dht11;
typedef struct __MDHT11
{
    void *this;
    /**
     * @brief 获取温度和湿度
     * @param temp 温度
     * @param humi 湿度
     * @return 0:成功 1:失败
     * 示例：
        u8 temp = 0;
        u8 humi = 0;

        dht11.get(&dht11, &temp, &humi);
    */
    u8 (*get)(const c_dht11 *this,u8 *temp,u8 *humi);
}c_dht11;

/**
 * @brief 创建c_dht11对象
 * @param GPIOx GPIO port
 * @param GPIO_Pin GPIO pin
 * @return c_dht11对象
 * 示例：
    c_dht11 dht11 = {0};
    
    dht11 = dht11_create(GPIOB, GPIO_Pin_8);
*/
c_dht11 dht11_create(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif
