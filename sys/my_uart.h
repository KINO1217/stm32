#ifndef __MY_UART_H__
#define __MY_UART_H__
#include "stm32f10x.h"

/**
 * @author KINO
 * @date 2024/4/18
 * @copyright Copyright (c) 2024 KINO
 * 串口驱动程序，可随意使用
 */

#define MAX_RECV_BUF_SIZE 64

enum MY_UART_ID
{
    MY_UART_ID_USART1 = 0,
    MY_UART_ID_USART2,
    MY_UART_ID_USART3
};

enum MY_UART_RECV_MODE
{
    ISR_MODE = 0, // 中断接收模式
    DMA_MODE      // DMA接收模式，暂时未添加
};

typedef struct __MY_UART
{
    /**
     * @brief 初始化串口
     * @param uartId 串口ID
     * @param band 波特率
     * @param recvMode 接收模式
     */
    void (*init)(u8 uartId, int band, u8 recvMode);

    /**
     * @brief 设置回调函数
     * @param uartId 串口ID
     * @param data 接收的数据
     * @param dataLen 接收的数据长度
     */
    void (*setCallback)(u8 uartId, void (*callback)(u8 *data, int dataLen));

    /**
     * @brief 发送数据
     * @param uartId 串口ID
     * @param data 发送的数据
     * @param data_len 发送的数据长度
     */
    void (*send)(u8 uartId, u8 *data, int data_len);
} c_my_uart;

extern const c_my_uart my_uart; //单例对象

#endif
