#ifndef __QF_PWM_H__
#define __QF_PWM_H__

#include "stm32f10x.h"

typedef enum {
    TIM1_CH1 = 0,
    TIM1_CH2,
    TIM1_CH3,
    TIM1_CH4,
    TIM2_CH1,
    TIM2_CH2,
    TIM2_CH3,
    TIM2_CH4,
    TIM3_CH1,
    TIM3_CH2,
    TIM3_CH3,
    TIM3_CH4,
    TIM4_CH1,
    TIM4_CH2,
    TIM4_CH3,
    TIM4_CH4,
} PWM_CH;

void QF_PWM1_Init(u16 psc, u16 arr);
void QF_PWM2_Init(u16 psc, u16 arr);
void QF_PWM3_Init(u16 psc, u16 arr);
void QF_PWM4_Init(u16 psc, u16 arr);
void QF_PWM_SetDuty(u8 ch, u16 duty);

#endif
