#include "qf_uart.h"

u8 UART1_RX_BUF[UART1_RX_BUF_SIZE] = { 0 };
u16 UART1_RX_LEN = 0;
void (*UART1_Callback)(u8* data, u16 data_len);
u8 UART2_RX_BUF[UART2_RX_BUF_SIZE] = { 0 };
u16 UART2_RX_LEN = 0;
void (*UART2_Callback)(u8* data, u16 data_len);
u8 UART3_RX_BUF[UART3_RX_BUF_SIZE] = { 0 };
u16 UART3_RX_LEN = 0;
void (*UART3_Callback)(u8* data, u16 data_len);

void QF_UART1_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化串口对应的端口

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure); // 初始化串口

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 设置中断优先级

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void QF_UART2_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化串口对应的端口

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure); // 初始化串口

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 设置中断优先级

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

    USART_Cmd(USART2, ENABLE);
}

void QF_UART3_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化串口对应的端口

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART3, &USART_InitStructure); // 初始化串口

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); // 设置中断优先级

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

    USART_Cmd(USART3, ENABLE);
}

void USART1_IRQHandler(void) // 串口1中断函数，编写中断逻辑
{
    uint16_t temp;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        temp = USART_ReceiveData(USART1);

        if (UART1_RX_LEN >= UART1_RX_BUF_SIZE) // 防止缓冲区溢出
            return;

        UART1_RX_BUF[UART1_RX_LEN] = (u8)temp;
        UART1_RX_LEN++;
    } else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        temp = USART_ReceiveData(USART1); // 获取一次数据清除中断

        UART1_Callback(UART1_RX_BUF, UART1_RX_LEN);
        memset((char*)UART1_RX_BUF, 0, sizeof(UART1_RX_BUF));
        UART1_RX_LEN = 0;
    }
}

void USART2_IRQHandler(void)
{
    uint16_t temp;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        temp = USART_ReceiveData(USART2);

        if (UART2_RX_LEN >= UART2_RX_BUF_SIZE) // 防止缓冲区溢出
            return;

        UART2_RX_BUF[UART2_RX_LEN] = (u8)temp;
        UART2_RX_LEN++;
    } else if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        temp = USART_ReceiveData(USART2); // 获取一次数据清除中断

        UART2_Callback(UART2_RX_BUF, UART2_RX_LEN);
        memset((char*)UART2_RX_BUF, 0, sizeof(UART2_RX_BUF));
        UART2_RX_LEN = 0;
    }
}

void USART3_IRQHandler(void)
{
    uint16_t temp;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        temp = USART_ReceiveData(USART3);

        if (UART3_RX_LEN >= UART3_RX_BUF_SIZE) // 防止缓冲区溢出
            return;

        UART3_RX_BUF[UART3_RX_LEN] = (u8)temp;
        UART3_RX_LEN++;
    } else if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) {
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
        temp = USART_ReceiveData(USART3); // 获取一次数据清除中断

        UART3_Callback(UART3_RX_BUF, UART3_RX_LEN);
        memset((char*)UART3_RX_BUF, 0, sizeof(UART3_RX_BUF));
        UART3_RX_LEN = 0;
    }
}

void QF_UART_Printf(USART_TypeDef* uartx, u8* data, ...)
{
    char tempBuf[256] = { 0 };

    va_list vaList;
    va_start(vaList, data);
    vsprintf(tempBuf, (char*)data, vaList);
    va_end(vaList);

    for (int i = 0; i < strlen(tempBuf); i++) {
        while (USART_GetFlagStatus(uartx, USART_FLAG_TXE) == RESET)
            ;
        USART_SendData(uartx, (uint16_t)tempBuf[i]);
        while (USART_GetFlagStatus(uartx, USART_FLAG_TC) == RESET)
            ;
    }
}
