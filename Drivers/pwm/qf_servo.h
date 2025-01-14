#ifndef __QF_SERVO_H__
#define __QF_SERVO_H__

#include "math.h"
#include "qf_delay.h"
#include "qf_pwm.h"
#include "qf_sys.h"
#include "stm32f10x.h"

/**
 * PWM精准控制
 */
void QF_SERVO_Init(void);
void QF_SERVO_SetAngle(u8 ch, float angle);

/**
 * 模拟PWM模糊控制
 */
#define GPIO_PORT GPIOA
#define GPIO_PIN GPIO_Pin_0

#define PORT_OUT PAout(0)

void QF_SERVO_Init1(void);
void QF_SERVO_90(void);
void QF_SERVO_0(void);

#endif
