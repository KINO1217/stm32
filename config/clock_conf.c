#include "clock_conf.h"

void vClockEnable(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
        RCC_APB2Periph_GPIOB|
        RCC_APB2Periph_GPIOC|
        RCC_APB2Periph_AFIO|
        RCC_APB2Periph_USART1
        , ENABLE);
    
    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_USART2|
        RCC_APB1Periph_USART3
        , ENABLE);
}
