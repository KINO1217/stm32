#include "qf_sim900a.h"
#include "qf_timer.h"
#include "qf_uart.h"

static u8 step = 0;
char mtel[15];
char mmessage[SIM900A_MESSAGE_LEN];

void QF_SIM900A_Send(u8* tel, u8* mess)
{
    if (step == 0) { // 第一次执行
        memset(mtel, 0, sizeof(mtel)); // 拷贝电话号
        strcpy(mtel, (char*)tel);

        memset(mmessage, 0, sizeof(mmessage)); // 拷贝消息
        strcpy(mmessage, (char*)mess);

        QF_UART_Printf(USART1, (u8*)"AT+CMGF=1\r\n");
        TIM_Cmd(TIM1, ENABLE);
        step = 1;
    }
}

void SIM900A_UartCallback(u8* data, u16 data_len)
{
}

void SIM900A_TimerCallback(void) // 执行发送
{
    switch (step) {
    case 1:
        QF_UART_Printf(USART1, (u8*)"AT+CSCS=\"GSM\"\r\n");
        step++;
        break;
    case 2:
        QF_UART_Printf(USART1, (u8*)"AT+CMGS=\"%s\"\r\n", mtel);
        step++;
        break;
    case 3:
        QF_UART_Printf(USART1, (u8*)"%s%c\r\n", mmessage, 0x1A);
        step = 0;
        TIM_Cmd(TIM1, DISABLE);
        break;
    }
}

void QF_SIM900A_Init(void)
{
    QF_UART1_Init(115200);
    UART1_Callback = SIM900A_UartCallback; // 做接收但不处理，防止回调函数未定义卡死

    QF_TIMER1_Init(720, 50000); // 500ms
    TIM_Cmd(TIM1, DISABLE);
    QF_TIMER1_Callback = SIM900A_TimerCallback;
}
