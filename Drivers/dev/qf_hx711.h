#ifndef __QF_HX711_H__
#define __QF_HX711_H__

#include "qf_sys.h"
#include "stm32f10x.h"

#define SMP_NUM 1

#define HX711SCK_PORT GPIOB
#define HX711SCK_PIN GPIO_Pin_9
#define HX711DAT_PORT GPIOB
#define HX711DAT_PIN GPIO_Pin_8

#define HXSCL_OUT PBout(9)
#define HXDAT_OUT PBout(8)
#define HXDAT_IN PBin(8)

void QF_HX711_Init(void);
void QF_HX711_GetZero(void);
float QF_HX711_GetWeight(void);

#endif
