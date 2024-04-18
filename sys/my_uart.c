#include "my_uart.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if 0
    #pragma import(__use_no_semihosting);

    void _sys_exit(int x)
    {
        x = x;
    }

    struct __FILE
    {
        int handle;
    };
    FILE __stdout;
#endif

struct __UART_CALLBACK
{
    u8 recvBuf[MAX_RECV_BUF_SIZE+1];
    int recvLen;
    void (*callback)(u8 *data, int dataLen);
} uartCallback[3];

static void uartInit(u8 uartId, int band, u8 recvMode);
static void setCallback(u8 uartId, void (*callback)(u8 *data, int dataLen));
static void send(u8 uartId, u8 *data, int data_len);

const c_my_uart my_uart = {uartInit,setCallback,send};
static void uartInit(u8 uartId, int band, u8 recvMode)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    switch (uartId)
    {
        case MY_UART_ID_USART1:
            //配置GPIO端口
            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStruct);

            //初始化串口1
            USART_InitStruct.USART_BaudRate = band;
            USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
            USART_InitStruct.USART_Parity = USART_Parity_No;
            USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_Init(USART1, &USART_InitStruct);

            //使能串口1
            USART_Cmd(USART1, ENABLE);

            //配置中断NVIC
            NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
            NVIC_Init(&NVIC_InitStruct);

            if(recvMode == ISR_MODE)
            {
                USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 接收中断
                USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 空闲中断
            }
            break;
        case MY_UART_ID_USART2:
            //配置GPIO端口
            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStruct);

            //初始化串口2
            USART_InitStruct.USART_BaudRate = band;
            USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
            USART_InitStruct.USART_Parity = USART_Parity_No;
            USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_Init(USART2, &USART_InitStruct);

            //使能串口2
            USART_Cmd(USART2, ENABLE);

            //配置中断NVIC
            NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
            NVIC_Init(&NVIC_InitStruct);

            if(recvMode == ISR_MODE)
            {
                USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 接收中断
                USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); // 空闲中断
            }
            break;
        case MY_UART_ID_USART3:
            //配置GPIO端口
            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOB, &GPIO_InitStruct);

            GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOB, &GPIO_InitStruct);

            //初始化串口3
            USART_InitStruct.USART_BaudRate = band;
            USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
            USART_InitStruct.USART_StopBits = USART_StopBits_1;
            USART_InitStruct.USART_Parity = USART_Parity_No;
            USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_Init(USART3, &USART_InitStruct);

            //使能串口3
            USART_Cmd(USART3, ENABLE);

            //配置中断NVIC
            NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
            NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
            NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
            NVIC_Init(&NVIC_InitStruct);

            if(recvMode == ISR_MODE)
            {
                USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 接收中断
                USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); // 空闲中断
            }
            break;
    }
}

static void setCallback(u8 uartId, void (*callback)(u8 *data, int dataLen))
{
    uartCallback[uartId].callback = callback;
}

static void send(u8 uartId, u8 *data, int data_len)
{
    switch (uartId)
    {
        case MY_UART_ID_USART1:
            for(int i=0;i<data_len;i++)
            {
                while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
                USART_SendData(USART1, data[i]);
                while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
            }
            break;
        case MY_UART_ID_USART2:
            for(int i=0;i<data_len;i++)
            {
                while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
                USART_SendData(USART2, data[i]);
                while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET);
            }
            break;
        case MY_UART_ID_USART3:
            for(int i=0;i<data_len;i++)
            {
                while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
                USART_SendData(USART3, data[i]);
                while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET);
            }
            break;
    }
}

int fputc(int ch,FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (u8)ch);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
    return 0;
}

void USART1_IRQHandler(void)
{
    uint16_t temp;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);

        temp = USART_ReceiveData(USART1);
        uartCallback[MY_UART_ID_USART1].recvBuf[uartCallback[MY_UART_ID_USART1].recvLen] = temp;
        uartCallback[MY_UART_ID_USART1].recvLen++;

        if(uartCallback[MY_UART_ID_USART1].recvLen >= MAX_RECV_BUF_SIZE)
        {
            uartCallback[MY_UART_ID_USART1].callback(uartCallback[MY_UART_ID_USART1].recvBuf,uartCallback[MY_UART_ID_USART1].recvLen);
            uartCallback[MY_UART_ID_USART1].recvLen = 0;
            memset((char*)uartCallback[MY_UART_ID_USART1].recvBuf,0,MAX_RECV_BUF_SIZE);
        }
    }
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART1,USART_IT_IDLE);
        temp = USART_ReceiveData(USART1);

        uartCallback[MY_UART_ID_USART1].callback(uartCallback[MY_UART_ID_USART1].recvBuf,uartCallback[MY_UART_ID_USART1].recvLen);
        uartCallback[MY_UART_ID_USART1].recvLen = 0;
        memset((char*)uartCallback[MY_UART_ID_USART1].recvBuf,0,MAX_RECV_BUF_SIZE);
    }
}

void USART2_IRQHandler(void)
{
    uint16_t temp;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);

        temp = USART_ReceiveData(USART2);
        uartCallback[MY_UART_ID_USART2].recvBuf[uartCallback[MY_UART_ID_USART2].recvLen] = temp;
        uartCallback[MY_UART_ID_USART2].recvLen++;

        if(uartCallback[MY_UART_ID_USART2].recvLen >= MAX_RECV_BUF_SIZE)
        {
            uartCallback[MY_UART_ID_USART2].callback(uartCallback[MY_UART_ID_USART2].recvBuf,uartCallback[MY_UART_ID_USART2].recvLen);
            uartCallback[MY_UART_ID_USART2].recvLen = 0;
            memset((char*)uartCallback[MY_UART_ID_USART2].recvBuf,0,MAX_RECV_BUF_SIZE);
        }
    }
    else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART2,USART_IT_IDLE);
        temp = USART_ReceiveData(USART2);

        uartCallback[MY_UART_ID_USART2].callback(uartCallback[MY_UART_ID_USART2].recvBuf,uartCallback[MY_UART_ID_USART2].recvLen);
        uartCallback[MY_UART_ID_USART2].recvLen = 0;
        memset((char*)uartCallback[MY_UART_ID_USART2].recvBuf,0,MAX_RECV_BUF_SIZE);
    }
}

void USART3_IRQHandler(void)
{
    uint16_t temp;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);

        temp = USART_ReceiveData(USART3);
        uartCallback[MY_UART_ID_USART3].recvBuf[uartCallback[MY_UART_ID_USART3].recvLen] = temp;
        uartCallback[MY_UART_ID_USART3].recvLen++;

        if(uartCallback[MY_UART_ID_USART3].recvLen >= MAX_RECV_BUF_SIZE)
        {
            uartCallback[MY_UART_ID_USART3].callback(uartCallback[MY_UART_ID_USART3].recvBuf,uartCallback[MY_UART_ID_USART3].recvLen);
            uartCallback[MY_UART_ID_USART3].recvLen = 0;
            memset((char*)uartCallback[MY_UART_ID_USART3].recvBuf,0,MAX_RECV_BUF_SIZE);
        }
    }
    else if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        USART_ClearITPendingBit(USART3,USART_IT_IDLE);
        temp = USART_ReceiveData(USART3);

        uartCallback[MY_UART_ID_USART3].callback(uartCallback[MY_UART_ID_USART3].recvBuf,uartCallback[MY_UART_ID_USART3].recvLen);
        uartCallback[MY_UART_ID_USART3].recvLen = 0;
        memset((char*)uartCallback[MY_UART_ID_USART3].recvBuf,0,MAX_RECV_BUF_SIZE);
    }
}
