#include "qf_servo.h"

void QF_SERVO_Init(void)
{
    QF_PWM2_Init(72, 20000);
}

void QF_SERVO_Init1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);
}

void QF_SERVO_90(void)
{
    for (u8 i = 0; i < 10; i++) {
        PORT_OUT = 1;
        QF_DELAY_Us(1500);
        PORT_OUT = 0;
        QF_DELAY_Us(18500);
    }
}

void QF_SERVO_0(void)
{
    for (u8 i = 0; i < 10; i++) {
        PORT_OUT = 1;
        QF_DELAY_Us(500);
        PORT_OUT = 0;
        QF_DELAY_Us(19500);
    }
}

void QF_SERVO_SetAngle(u8 ch, float angle)
{
    QF_PWM_SetDuty(ch, (int)round(11.111111 * angle + 500));
}
