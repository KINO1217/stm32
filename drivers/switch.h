#ifndef __SWITCH_H__
#define __SWITCH_H__
#include <stm32f10x.h>

enum __SWITCH_STATE{
    SWITCH_LOW=0,
    SWITCH_HIGHT
};

typedef struct __SWITCH c_switch;
typedef struct __SWITCH
{
    /*保存GPIO结构体对象*/
    void *this;
    /*
    * 设置GPIO口输出高低电平
    * 示例：
            led.set(&led,SWITCH_HIGHT);
    */
    void (*set)(const c_switch* this,u8 state);
}c_switch;
/*
 * 创建端口输出对象c_switch
 * 示例：
        c_switch led = {0};
        led = switch_create(GPIOC,GPIO_Pin_13);
*/
c_switch switch_create(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
#endif
