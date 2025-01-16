#ifndef __QF_UART_H__
#define __QF_UART_H__

#include "stdarg.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "string.h"

#define UART1_RX_BUF_SIZE 16
extern void (*UART1_Callback)(u8* data, u16 data_len);
#define UART2_RX_BUF_SIZE 16
extern void (*UART2_Callback)(u8* data, u16 data_len);
#define UART3_RX_BUF_SIZE 256
extern void (*UART3_Callback)(u8* data, u16 data_len);

void QF_UART1_Init(uint32_t baud);
void QF_UART2_Init(uint32_t baud);
void QF_UART3_Init(uint32_t baud);
void QF_UART_Printf(USART_TypeDef* uartx, u8* data, ...);
void QF_UART_SendData(USART_TypeDef* uartx, u8* data, u8 data_len);

#endif
