#include "qf_conf.h"

void QF_RCC_Conf(void)
{
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA
            | RCC_APB2Periph_GPIOB
            | RCC_APB2Periph_USART1
            | RCC_APB2Periph_TIM1
            | RCC_APB2Periph_ADC1,
        ENABLE);

    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_USART2
            | RCC_APB1Periph_USART3
            | RCC_APB1Periph_TIM2
            | RCC_APB1Periph_TIM3
            | RCC_APB1Periph_TIM4,
        ENABLE);
}
