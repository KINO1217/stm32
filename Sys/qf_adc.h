#ifndef __QF_ADC_H__
#define __QF_ADC_H__

#include "stm32f10x.h"

#define MEAN_CNT 1

void QF_ADC_Init(void);
u8 QF_ADC_GetV(u8 ch);

#endif
