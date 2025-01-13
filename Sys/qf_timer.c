#include "qf_timer.h"

void (*QF_TIMER1_Callback)(void);
void (*QF_TIMER2_Callback)(void);
void (*QF_TIMER3_Callback)(void);
void (*QF_TIMER4_Callback)(void);

void QF_TIMER1_Init(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // �߼���ʱ����Ҫ�������ã�ʱ�������Ƿ���з�������
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_Period = arr - 1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ����жϱ�־λ
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // ʹ�ܸ����ж�
    TIM_Cmd(TIM1, ENABLE); // ʹ�ܶ�ʱ��
}

void QF_TIMER2_Init(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // �߼���ʱ����Ҫ�������ã�ʱ�������Ƿ���з�������
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_Period = arr - 1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ����жϱ�־λ
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // ʹ�ܸ����ж�
    TIM_Cmd(TIM2, ENABLE); // ʹ�ܶ�ʱ��
}

void QF_TIMER3_Init(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // �߼���ʱ����Ҫ�������ã�ʱ�������Ƿ���з�������
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_Period = arr - 1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // ʹ�ܸ����ж�
    TIM_Cmd(TIM3, ENABLE); // ʹ�ܶ�ʱ��
}

void QF_TIMER4_Init(u16 psc, u16 arr)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; // �߼���ʱ����Ҫ�������ã�ʱ�������Ƿ���з�������
    TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStructure.TIM_Period = arr - 1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // ����жϱ�־λ
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // ʹ�ܸ����ж�
    TIM_Cmd(TIM4, ENABLE); // ʹ�ܶ�ʱ��
}

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ����жϱ�־λ

        QF_TIMER1_Callback();
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ����жϱ�־λ

        QF_TIMER2_Callback();
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ

        QF_TIMER3_Callback();
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // ����жϱ�־λ

        QF_TIMER4_Callback();
    }
}
