#ifndef __QF_MOTOR_H__
#define __QF_MOTOR_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

#define INA1_PORT GPIOA
#define INA1_PIN GPIO_Pin_0
#define INB2_PORT GPIOA
#define INB2_PIN GPIO_Pin_1
#define INC3_PORT GPIOA
#define INC3_PIN GPIO_Pin_2
#define IND4_PORT GPIOA
#define IND4_PIN GPIO_Pin_3

#define INA1 PAout(0)
#define INB2 PAout(1)
#define INC3 PAout(2)
#define IND4 PAout(3)

void QF_MOTOR_Init(void);
void QF_MOTOR_RotateZ(void);
void QF_MOTOR_RotateF(void);

#endif
