#ifndef __QF_ADC_H__
#define __QF_ADC_H__

#include "stm32f10x.h"

void QF_ADC_Init(void);
float QF_ADC_GetV(u8 ch);

#endif
