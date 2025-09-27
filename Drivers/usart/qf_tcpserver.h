#ifndef __QF_TCPSERVER_H__
#define __QF_TCPSERVER_H__

#include "stm32f10x.h"

#define TCPSERVER_RECV_LENMAX 250

void QF_TCPSERVER_Init(void);
void QF_RECV_Callback(u8* data,u16 data_len);
extern void (*QF_TCPSERVER_Callback)(u8* data,u16 data_len);
void QF_TCPSERVER_SendString(u8* data,u16 data_len);

#endif
