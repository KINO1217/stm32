#ifndef __QF_HC_SR04_H__
#define __QF_HC_SR04_H__

#include "qf_sys.h"
#include "stm32f10x.h"

#define TRIG_PORT GPIOB
#define TRIG_PIN GPIO_Pin_9
#define ECHO_PORT GPIOB
#define ECHO_PIN GPIO_Pin_8

#define TRIG_OUT PBout(9)
#define ECHO_IN PBin(8)

void QF_HCSR04_Init(void);
void QF_HCSR04_GetDis(float* dis);

#endif
