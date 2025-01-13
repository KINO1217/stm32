#ifndef __QF_DELAY_H__
#define __QF_DELAY_H__

#include "stm32f10x.h"

void QF_DELAY_Init(void);
void QF_DELAY_Us(u32 nus);
void QF_DELAY_Ms(u32 nms);

#endif
