#ifndef __HC_SR04_H__
#define __HC_SR04_H__
#include <stm32f10x.h>

/**
 * HC-SR04超声测距模块驱动文件
 * @author KINO
*/

typedef struct __CHCSR04 c_hcsr04;
typedef struct __CHCSR04
{
    void *this;
    /**
     * 获取距离，根据温度补偿计算，单位CM
     * 示例：
        float dis = 0.0f;
        
        hc04.get(&hc04,&dis,25.0f);
    */
    void (*get)(c_hcsr04 *this,float *dis,float temp);
}c_hcsr04;

/**
 * 创建c_hcsr04对象
 * 示例：
    c_hcsr04 hc04 = {0};
    hc04 = hcsr04_create(GPIOB,GPIO_Pin_5,GPIOB,GPIO_Pin_6);
*/
c_hcsr04 hcsr04_create(GPIO_TypeDef* GPIO_Trig,uint16_t Trig_Pin,GPIO_TypeDef* GPIO_Echo,uint16_t Echo_Pin);

#endif
