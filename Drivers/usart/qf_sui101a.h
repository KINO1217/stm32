#ifndef __QF_SUI101A_H__
#define __QF_SUI101A_H__

#include "stm32f10x.h"

void QF_SUI101A_Init(void);
void QF_SUI101A_GetData(float* V, float* I, float* P, float* PF, float* F, float* W);
void QF_SUI101A_ClearPower(void); // 清除电量累计

#endif
