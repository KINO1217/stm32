#ifndef __MY_PWM_H__
#define __MY_PWM_H__
#include <stm32f10x.h>

enum __TIMID
{
    TIM_ID_1=0,
    TIM_ID_2,
    TIM_ID_3,
    TIM_ID_4,
};

enum __CHID
{
    CH_ID_1=0,
    CH_ID_2,
    CH_ID_3,
    CH_ID_4,
};

typedef struct __MPWM
{
    void (*timInit)(u8 timId,u16 psc,u16 arr);
    void (*chInit)(u8 timId,u8 chId);
    void (*chSet)(u8 timId,u8 chId,u16 duty);
}c_my_pwm;

extern const c_my_pwm my_pwm;

#endif
