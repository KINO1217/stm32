#include "qf_pwm.h"

/**
 * TIM1 PA8 PA9 PA10 PA11
 * TIM2 PA0 PA1 PA2 PA3
 * TIM3 PA6 PA7 PB0 PB1
 * TIM4 PB6 PB7 PB8 PB9
 */

void QF_PWM1_Init(u16 psc, u16 arr)
{
    // 初始化定时器时基
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0; // 高级定时器使用，时间是否倍数
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化输出比较参数
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    // TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    // TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    // TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    TIM_Cmd(TIM1, ENABLE); // 使能定时器
    TIM_CtrlPWMOutputs(TIM1, ENABLE); // 高级定时器必须开
}

void QF_PWM2_Init(u16 psc, u16 arr)
{
    // 初始化定时器时基
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0; // 高级定时器使用，时间是否倍数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化输出比较参数
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    // TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    // TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    // TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE); // 使能定时器
}

void QF_PWM3_Init(u16 psc, u16 arr)
{
    // 初始化定时器时基
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0; // 高级定时器使用，时间是否倍数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化输出比较参数
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    // TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    // TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    // TIM_OC4Init(TIM3, &TIM_OCInitStructure);

    TIM_Cmd(TIM3, ENABLE); // 使能定时器
}

void QF_PWM4_Init(u16 psc, u16 arr)
{
    // 初始化定时器时基
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0; // 高级定时器使用，时间是否倍数
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化输出比较参数
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);

    // TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    // TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    // TIM_OC4Init(TIM4, &TIM_OCInitStructure);

    TIM_Cmd(TIM4, ENABLE); // 使能定时器
}

void QF_PWM_SetDuty(u8 ch, u16 duty)
{
    switch (ch) {
    case TIM1_CH1:
        TIM_SetCompare1(TIM1, duty);
        break;
    case TIM1_CH2:
        TIM_SetCompare2(TIM1, duty);
        break;
    case TIM1_CH3:
        TIM_SetCompare3(TIM1, duty);
        break;
    case TIM1_CH4:
        TIM_SetCompare4(TIM1, duty);
        break;
    case TIM2_CH1:
        TIM_SetCompare1(TIM2, duty);
        break;
    case TIM2_CH2:
        TIM_SetCompare2(TIM2, duty);
        break;
    case TIM2_CH3:
        TIM_SetCompare3(TIM2, duty);
        break;
    case TIM2_CH4:
        TIM_SetCompare4(TIM2, duty);
        break;
    case TIM3_CH1:
        TIM_SetCompare1(TIM3, duty);
        break;
    case TIM3_CH2:
        TIM_SetCompare2(TIM3, duty);
        break;
    case TIM3_CH3:
        TIM_SetCompare3(TIM3, duty);
        break;
    case TIM3_CH4:
        TIM_SetCompare4(TIM3, duty);
        break;
    case TIM4_CH1:
        TIM_SetCompare1(TIM4, duty);
        break;
    case TIM4_CH2:
        TIM_SetCompare2(TIM4, duty);
        break;
    case TIM4_CH3:
        TIM_SetCompare3(TIM4, duty);
        break;
    case TIM4_CH4:
        TIM_SetCompare4(TIM4, duty);
        break;
    default:
        break;
    }
}
