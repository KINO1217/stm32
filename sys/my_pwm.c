#include "my_pwm.h"

static void timInit(u8 timId, u16 psc, u16 arr);
static void chInit(u8 timId, u8 chId);
static void chSet(u8 timId, u8 chId, u16 duty);

const c_my_pwm my_pwm = {timInit, chInit, chSet};

/**
 * TIM1：CH1:PA8,CH2:PA9,CH3:PA10,CH4:PA11
 * TIM2：CH1:PA0,CH2:PA1,CH3:PA2,CH4:PA3
 * TIM3：CH1:PA6,CH2:PA7,CH3:PB0,CH4:PB1
 * TIM4：CH1:PB6,CH2:PB7,CH3:PB8,CH4:PB9
 */

static void timInit(u8 timId, u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    if (timId == TIM_ID_1)
    {
        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
    }
    else if (timId == TIM_ID_2)
    {
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    }
    else if (timId == TIM_ID_3)
    {
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
    }
    else if (timId == TIM_ID_4)
    {
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);
    }
}

static void chInit(u8 timId, u8 chId)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCStructInit(&TIM_OCInitStruct); // 结构体参数需要全部初始化一遍
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;

    if (timId == TIM_ID_1)
    {
        if (chId == CH_ID_1)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC1Init(TIM1, &TIM_OCInitStruct);

            TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_2)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,DISABLE); //时钟冲突，失能

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC2Init(TIM1, &TIM_OCInitStruct);

            TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_3)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC3Init(TIM1, &TIM_OCInitStruct);

            TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_4)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC4Init(TIM1, &TIM_OCInitStruct);

            TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
        }

        TIM_ARRPreloadConfig(TIM1, ENABLE);
        TIM_Cmd(TIM1, ENABLE);
        TIM_CtrlPWMOutputs(TIM1, ENABLE); // 高级定时器必须开
    }
    else if (timId == TIM_ID_2)
    {
        if (chId == CH_ID_1)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC1Init(TIM2, &TIM_OCInitStruct);

            TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_2)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC2Init(TIM2, &TIM_OCInitStruct);

            TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_3)
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,DISABLE); //时钟冲突，失能

            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC3Init(TIM2, &TIM_OCInitStruct);

            TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_4)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC4Init(TIM2, &TIM_OCInitStruct);

            TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
        }

        TIM_ARRPreloadConfig(TIM2, ENABLE);
        TIM_Cmd(TIM2, ENABLE);
    }
    else if (timId == TIM_ID_3)
    {
        if (chId == CH_ID_1)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC1Init(TIM3, &TIM_OCInitStruct);

            TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_2)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            TIM_OC2Init(TIM3, &TIM_OCInitStruct);

            TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_3)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC3Init(TIM3, &TIM_OCInitStruct);

            TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_4)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC4Init(TIM3, &TIM_OCInitStruct);

            TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
        }

        TIM_ARRPreloadConfig(TIM3, ENABLE);
        TIM_Cmd(TIM3, ENABLE);
    }
    else if (timId == TIM_ID_4)
    {
        if (chId == CH_ID_1)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC1Init(TIM4, &TIM_OCInitStruct);

            TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_2)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC2Init(TIM4, &TIM_OCInitStruct);

            TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_3)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC3Init(TIM4, &TIM_OCInitStruct);

            TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
        }
        else if (chId == CH_ID_4)
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
            GPIO_Init(GPIOB, &GPIO_InitStructure);

            TIM_OC4Init(TIM4, &TIM_OCInitStruct);

            TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
        }

        TIM_ARRPreloadConfig(TIM4, ENABLE);
        TIM_Cmd(TIM4, ENABLE);
    }
}

static void chSet(u8 timId, u8 chId, u16 duty)
{
    if (timId == TIM_ID_1)
    {
        if (chId == CH_ID_1)
        {
            TIM_SetCompare1(TIM1, duty);
        }
        else if (chId == CH_ID_2)
        {
            TIM_SetCompare2(TIM1, duty);
        }
        else if (chId == CH_ID_3)
        {
            TIM_SetCompare3(TIM1, duty);
        }
        else if (chId == CH_ID_4)
        {
            TIM_SetCompare4(TIM1, duty);
        }
    }
    else if (timId == TIM_ID_2)
    {
        if (chId == CH_ID_1)
        {
            TIM_SetCompare1(TIM2, duty);
        }
        else if (chId == CH_ID_2)
        {
            TIM_SetCompare2(TIM2, duty);
        }
        else if (chId == CH_ID_3)
        {
            TIM_SetCompare3(TIM2, duty);
        }
        else if (chId == CH_ID_4)
        {
            TIM_SetCompare4(TIM2, duty);
        }
    }
    else if (timId == TIM_ID_3)
    {
        if (chId == CH_ID_1)
        {
            TIM_SetCompare1(TIM3, duty);
        }
        else if (chId == CH_ID_2)
        {
            TIM_SetCompare2(TIM3, duty);
        }
        else if (chId == CH_ID_3)
        {
            TIM_SetCompare3(TIM3, duty);
        }
        else if (chId == CH_ID_4)
        {
            TIM_SetCompare4(TIM3, duty);
        }
    }
    else if (timId == TIM_ID_4)
    {
        if (chId == CH_ID_1)
        {
            TIM_SetCompare1(TIM4, duty);
        }
        else if (chId == CH_ID_2)
        {
            TIM_SetCompare2(TIM4, duty);
        }
        else if (chId == CH_ID_3)
        {
            TIM_SetCompare3(TIM4, duty);
        }
        else if (chId == CH_ID_4)
        {
            TIM_SetCompare4(TIM4, duty);
        }
    }
}
