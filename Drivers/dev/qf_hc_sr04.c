#include "qf_hc_sr04.h"
#include "qf_delay.h"

void QF_HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TRIG_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(ECHO_PORT, &GPIO_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // 高级定时器需要进行设置，时间周期是否进行翻倍设置
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF - 1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_Cmd(TIM4, ENABLE);
}

void QF_HCSR04_GetDis(float* dis)
{
    u16 cnt = 0x0000;
    u16 time = 0;

    TRIG_OUT = 1;
    QF_DELAY_Us(10);
    TRIG_OUT = 0;

    while (!ECHO_IN && cnt < 471) {
        QF_DELAY_Us(9);
        cnt++;
    }
    if (cnt >= 471)
        return;

    TIM_SetCounter(TIM4, 0);

    cnt = 0;
    while (ECHO_IN && cnt < 471) // 最大72cm
    {
        QF_DELAY_Us(9);
        cnt++;
    }
    if (cnt >= 471)
        return;

    time = TIM_GetCounter(TIM4);

    *dis = (float)time * 340.0 / 20000;

    QF_DELAY_Ms(25);
}
