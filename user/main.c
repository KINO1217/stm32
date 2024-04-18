#include "driver_conf.h"

void uart1_recv_callback(u8 *data, int dataLen)
{
    printf("%s-%d",data,dataLen);
}

void uart2_recv_callback(u8 *data, int dataLen)
{
    my_uart.send(MY_UART_ID_USART2,data,dataLen);
}

void uart3_recv_callback(u8 *data, int dataLen)
{
    my_uart.send(MY_UART_ID_USART3,data,dataLen);
}
int main()
{
    vClockEnable();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级组
    vDelayInit();
    my_uart.init(MY_UART_ID_USART1,115200,ISR_MODE);
    my_uart.setCallback(MY_UART_ID_USART1,uart1_recv_callback);

    my_uart.init(MY_UART_ID_USART2,115200,ISR_MODE);
    my_uart.setCallback(MY_UART_ID_USART2,uart2_recv_callback);

    my_uart.init(MY_UART_ID_USART3,115200,ISR_MODE);
    my_uart.setCallback(MY_UART_ID_USART3,uart3_recv_callback);
    
    while(1)
    {
        
    }
}
