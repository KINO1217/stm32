#ifndef __QF_DHT11_H__
#define __QF_DHT11_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

#define DAT_PORT GPIOA
#define DAT_PIN GPIO_Pin_0

#define DAT_OUT PAout(0)
#define DAT_IN PAin(0)

void QF_DHT11_Init(void);
u8 QF_DHT11_GetData(u8* temp, u8* humi);
void QF_DATLINEMODE(u8 mode);
void QF_DHT11_Start(void);
u8 QF_DHT11_GetByte(void);

#endif
