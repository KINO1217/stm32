#include "qf_martix_key.h"
#include "qf_delay.h"

static void QF_MARTIX_KEY_ModeChange(u8 mode);

void QF_MARTIX_KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = C1_PIN;
    GPIO_Init(C1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = C2_PIN;
    GPIO_Init(C2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = C3_PIN;
    GPIO_Init(C3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = C4_PIN;
    GPIO_Init(C4_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = R1_PIN;
    GPIO_Init(R1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R2_PIN;
    GPIO_Init(R2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R3_PIN;
    GPIO_Init(R3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R4_PIN;
    GPIO_Init(R4_PORT, &GPIO_InitStructure);

    C1_Out(0);
    C2_Out(0);
    C3_Out(0);
    C4_Out(0);

    QF_DELAY_Us(500);
}

u8 QF_MARTIX_KEY_Scan(u8 mode)
{
    static u8 keyMode = 0;
    u8 row = 0;
    u8 col = 0;

    if (mode == 1) {
        keyMode = 1;
    }

    QF_MARTIX_KEY_ModeChange(0); // 列为输出，行为输入，确认按下行

    if ((R1_In == 0 || R2_In == 0 || R3_In == 0 || R4_In == 0) && keyMode == 1) {
        QF_DELAY_Ms(20); // 消抖
        if (R1_In == 0 || R2_In == 0 || R3_In == 0 || R4_In == 0) {
            if (R1_In == 0) {
                row = 4;
            } else if (R2_In == 0) {
                row = 3;
            } else if (R3_In == 0) {
                row = 2;
            } else if (R4_In == 0) {
                row = 1;
            }

            keyMode = 0;
        }
    } else if (R1_In == 1 && R2_In == 1 && R3_In == 1 && R4_In == 1 && keyMode == 0) {
        keyMode = 1;
    }

    QF_MARTIX_KEY_ModeChange(1); // 行为输出，列为输入，确认按下列

    if (row != 0) {
        if (C1_In == 0) {
            col = 4;
        } else if (C2_In == 0) {
            col = 3;
        } else if (C3_In == 0) {
            col = 2;
        } else if (C4_In == 0) {
            col = 1;
        }
    }

    if (row == 0) {
        return 0;
    } else {
        return ((row - 1) * 4 + col);
    }
}

static void QF_MARTIX_KEY_ModeChange(u8 mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (mode == 0) {
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = C1_PIN;
        GPIO_Init(C1_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C2_PIN;
        GPIO_Init(C2_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C3_PIN;
        GPIO_Init(C3_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C4_PIN;
        GPIO_Init(C4_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = R1_PIN;
        GPIO_Init(R1_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R2_PIN;
        GPIO_Init(R2_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R3_PIN;
        GPIO_Init(R3_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R4_PIN;
        GPIO_Init(R4_PORT, &GPIO_InitStructure);

        C1_Out(0);
        C2_Out(0);
        C3_Out(0);
        C4_Out(0);
    } else if (mode == 1) {
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = C1_PIN;
        GPIO_Init(C1_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C2_PIN;
        GPIO_Init(C2_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C3_PIN;
        GPIO_Init(C3_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = C4_PIN;
        GPIO_Init(C4_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = R1_PIN;
        GPIO_Init(R1_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R2_PIN;
        GPIO_Init(R2_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R3_PIN;
        GPIO_Init(R3_PORT, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = R4_PIN;
        GPIO_Init(R4_PORT, &GPIO_InitStructure);

        R1_Out(0);
        R2_Out(0);
        R3_Out(0);
        R4_Out(0);
    }

    QF_DELAY_Us(500);
}
