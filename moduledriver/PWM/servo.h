#ifndef __SERVO_H__
#define __SERVO_H__
#include <stm32f10x.h>

typedef struct __CSERVO c_servo;
typedef struct __CSERVO
{
    void *this;
    /**
     * @brief 设置角度
     * @param angle 角度值 0.0-180.0f
     * 示例：
        servo.set_angle(&servo,180.0f);
    */
    void (*set_angle)(const c_servo *this,float angle);
}c_servo;

/**
 * @brief 创建c_servo对象
 * @param timId 定时器id
 * @param channel 通道号
 * @return c_servo对象
 * 示例：
    c_servo servo = {0};

    servo = servo_create(TIM_ID_1,CH_ID_1);
*/
c_servo servo_create(u8 timId,u8 channel);

#endif
