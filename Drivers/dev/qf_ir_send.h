#ifndef __QF_IR_SEND_H__
#define __QF_IR_SEND_H__

#include "stm32f10x.h"
#include "qf_sys.h"
#include "qf_delay.h"

#define ENPORT GPIOB
#define ENPIN GPIO_Pin_9
#define ENOUT PBout(9)

void QF_IRSEND_Init(void);
void QF_IRSEND_SendData(u16 addrCode,u8 datas);

#endif
