#include "qf_ir_recv.h"

void (*QF_IRRECV_Callback)(u8* data);
int8_t QF_GetCode(void);

void QF_IRRECV_Init(void)
{
    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = IR_RECVIER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_RECVIER_GPIO, &GPIO_InitStructure);

    // 配置外部中断线
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

    // 配置外部中断
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    EXTI_Init(&EXTI_InitStruct);

    // 配置NVIC中断优先级
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3; // 数字越大优先级越低
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_Init(&NVIC_InitStruct);
}

int8_t QF_GetCode(void)
{
    uint16_t cnt = 0;
    uint8_t irData[4] = { 0 };

    // 检测引导码的低电平时间
    while (IR_IN == 0) {
        QF_DELAY_Us(10);
        cnt++;
        if (cnt > 1000) // 10ms超时检测
            return -1;
    }

    // 检验引导码的高电平时间
    cnt = 0;
    DISABLE_INT; // 关闭中断
    while (IR_IN == 1) {
        QF_DELAY_Us(10);
        cnt++;
        if (cnt > 500) {
            ENABLE_INT; // 开启中断
            return -1;
        }
    }

    // 接收4字节数据
    for (u8 i = 0; i < 4; i++) {
        for (u8 j = 0; j < 8; j++) {
            cnt = 0;
            while (IR_IN == 0) {
                QF_DELAY_Us(10);
                cnt++;
                if (cnt > 60) {
                    ENABLE_INT; // 开启中断
                    return -1;
                }
            }

            cnt = 0;
            while (IR_IN == 1) {
                QF_DELAY_Us(10);
                cnt++;
                if (cnt > 170) {
                    ENABLE_INT; // 开启中断
                    return -1;
                }
            }

            if (cnt > 60) {
                irData[i] |= (0x01 << j);
            }
        }
    }

    QF_DELAY_Us(600);

    if ((irData[0] + irData[1]) != 255 || (irData[2] + irData[3]) != 255) // 检验数据正确性
    {
        ENABLE_INT; // 开启中断
        return -3;
    }

    // 发送数据
    QF_IRRECV_Callback(irData);

    ENABLE_INT;
    return 0;
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
        EXTI_ClearITPendingBit(EXTI_Line0);

        // 触发中断事件
        int8_t ret = QF_GetCode();
        if (ret < 0)
            return;
    }
}
