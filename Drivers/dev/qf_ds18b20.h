#ifndef __QF_DS18B20_H__
#define __QF_DS18B20_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

#define DQ_PORT GPIOA
#define DQ_PIN GPIO_Pin_1

#define DQ_OUT PAout(1)
#define DQ_IN PAin(1)

void QF_DS18B20_Init(void);
void QF_DS18B20_GetTemp(float* temp);
void QF_DS18B20_DQLINEMODE(u8 mode);
u8 QF_DS18B20_Reset(void);
void QF_DS18B20_WriteByte(u8 dat);
u8 QF_DS18B20_ReadByte(void);

#endif
