#include "qf_sui101a.h"
#include "qf_delay.h"
#include "qf_uart.h"

float mVrms = 0.0; // 电压
float mIrms = 0.0; // 电流
float mP = 0.0; // 有功功率
float mPF = 0.0; // 功率因数
float mF = 0.0; // 频率
float mW = 0.0; // 累计电量
u8 sui101a_recv_flag = 0;

void QF_SUI101A_Callback(u8* data, u16 data_len)
{
    u32 tempData = 0;

    if (data[3] == 0x02) { // 接收到电压数据
        tempData = (data[6] << 24) + (data[7] << 16) + (data[8] << 8) + data[9];
        mVrms = (float)tempData / 1000.0; // V
        tempData = (data[10] << 24) + (data[11] << 16) + (data[12] << 8) + data[13];
        mIrms = (float)tempData / 1000.0; // A
        tempData = (data[14] << 24) + (data[15] << 16) + (data[16] << 8) + data[17];
        mP = (float)tempData / 1000.0; // W
        tempData = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
        mPF = (float)tempData / 10000.0;
        tempData = (data[22] << 24) + (data[23] << 16) + (data[24] << 8) + data[25];
        mF = (float)tempData / 1000.0; // Hz
        tempData = (data[26] << 24) + (data[27] << 16) + (data[28] << 8) + data[29];
        mW = (float)tempData / 10.0; // W·h

        sui101a_recv_flag = 1;
    }
}

void QF_SUI101A_Init(void)
{
    QF_UART1_Init(9600);
    UART1_Callback = QF_SUI101A_Callback;
}

void QF_SUI101A_GetData(float* V, float* I, float* P, float* PF, float* F, float* W)
{
    u8 send[] = { 0x55, 0x55, 0x01, 0x02, 0x00, 0x00, 0xAD };
    u16 time = 0;

    QF_UART_SendData(USART1, send, 7);

    while (sui101a_recv_flag == 0 && time < 500) {
        QF_DELAY_Us(10);
        time++;
    }

    *V = mVrms;
    *I = mIrms;
    *P = mP;
    *PF = mPF;
    *F = mF;
    *W = mW;

    sui101a_recv_flag = 0;
}

void QF_SUI101A_ClearPower(void)
{
    u8 send[] = { 0x55, 0x55, 0x01, 0xF3, 0x00, 0x02, 0x12, 0x34, 0xE6 };

    QF_UART_SendData(USART1, send, 9);

    QF_DELAY_Ms(5);
}
