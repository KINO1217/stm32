#include "my_timer.h"

struct __MTIMER
{
    void (*callback)(void);
} timers[4];

static void initTimer(u8 timerId, u16 psc, u16 arr);
static void setCallback(u8 timerId, void (*timerCallback)(void));

const c_my_timer my_timer = {initTimer, setCallback};

static void initTimer(u8 timerId, u16 psc, u16 arr)
{
    /**初始化定时器*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = arr - 1;
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    /**设置中断向量表*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    if(timerId == SYS_TIMER_1)
    {
        TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

        NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
        NVIC_Init(&NVIC_InitStruct);

        TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
        TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
        TIM_Cmd(TIM1,ENABLE);
    }
    else if (timerId == SYS_TIMER_2)
    {
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

        NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
        NVIC_Init(&NVIC_InitStruct);

        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
        TIM_Cmd(TIM2,ENABLE);
    }
    else if(timerId == SYS_TIMER_3)
    {
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

        NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_Init(&NVIC_InitStruct);

        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
        TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
        TIM_Cmd(TIM3,ENABLE);
    }
    else if(timerId == SYS_TIMER_4)
    {
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct);

        NVIC_InitStruct.NVIC_IRQChannel = TIM4_IRQn;
        NVIC_Init(&NVIC_InitStruct);

        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
        TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
        TIM_Cmd(TIM4,ENABLE);
    }
}

static void setCallback(u8 timerId, void (*timerCallback)(void))
{
    timers[timerId].callback = timerCallback;
}

void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1,TIM_IT_Update) == SET)
    {
        timers[0].callback();
        TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
    }
}

void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
    {
        timers[1].callback();
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    }
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
    {
        timers[2].callback();
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    }
}

void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4,TIM_IT_Update) == SET)
    {
        timers[3].callback();
        TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    }
}
