#include "qf_motor.h"

void QF_MOTOR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = INA1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(INA1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = INB2_PIN;
    GPIO_Init(INB2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = INC3_PIN;
    GPIO_Init(INC3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = IND4_PIN;
    GPIO_Init(IND4_PORT, &GPIO_InitStructure);
}

void QF_MOTOR_RotateZ(void)
{
    for (int i = 0; i < 128; i++) {
        INA1 = 1;
        INB2 = 0;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 1;
        INB2 = 1;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 1;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 1;
        INC3 = 1;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 1;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 1;
        IND4 = 1;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 0;
        IND4 = 1;
        QF_DELAY_Ms(1);
        INA1 = 1;
        INB2 = 0;
        INC3 = 0;
        IND4 = 1;
        QF_DELAY_Ms(1);
    }
}

void QF_MOTOR_RotateF(void)
{
    for (int i = 0; i < 128; i++) {
        INA1 = 1;
        INB2 = 0;
        INC3 = 0;
        IND4 = 1;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 0;
        IND4 = 1;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 1;
        IND4 = 1;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 0;
        INC3 = 1;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 1;
        INC3 = 1;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 0;
        INB2 = 1;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 1;
        INB2 = 1;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
        INA1 = 1;
        INB2 = 0;
        INC3 = 0;
        IND4 = 0;
        QF_DELAY_Ms(1);
    }
}
