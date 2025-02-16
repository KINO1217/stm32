#ifndef __QF_SIM900A_H__
#define __QF_SIM900A_H__

#include "stm32f10x.h"

#define SIM900A_MESSAGE_LEN 64

void QF_SIM900A_Init(void);
void QF_SIM900A_Send(u8* tel, u8* mess);

#endif
