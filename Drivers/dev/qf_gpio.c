#include "qf_gpio.h"

void QF_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    // GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void QF_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 QF_KEY_Scan(u8 mode)
{
    static u8 key_state = 1;
    if (mode)
        key_state = 1;

    if ((KEY1 == 0) && key_state) {
        QF_DELAY_Ms(20); // Ïû¶¶
        key_state = 0;

        if (KEY1 == 0)
            return KEY1_PRESS;

    } else if (KEY1 == 1 && key_state == 0)
        key_state = 1;

    return 0;
}
