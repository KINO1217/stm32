#ifndef __QF_GPS_H__
#define __QF_GPS_H__

#include "stm32f10x.h"

void QF_GPS_Init(void);
void QF_GPS_GetData(double* lon, double* lat);

#endif
