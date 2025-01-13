#ifndef __QF_GPIO_H__
#define __QF_GPIO_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

// GPIO_OUT/IN
#define LED PAout(4)

// KEY SET
#define KEY1 PBin(14)

#define KEY1_PRESS 1

void QF_GPIO_Init(void);
void QF_KEY_Init(void);
u8 QF_KEY_Scan(u8 mode);

#endif
