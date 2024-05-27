#ifndef __TIMER_H__
#define __TIMER_H__
#include <stm32f10x.h>

enum TIMER_ID
{
    SYS_TIMER_1 = 0,
    SYS_TIMER_2,
    SYS_TIMER_3,
    SYS_TIMER_4,
};

typedef struct __CTIMER
{
    /**
     * 初始化定时器
     */
    void (*initTimer)(u8 timerId, u16 psc, u16 arr);
    /**
     * 为定时器设置回调函数
    */
    void (*setCallback)(u8 timerId,void (*timerCallback)(void));
} c_my_timer;

extern const c_my_timer my_timer;

#endif
