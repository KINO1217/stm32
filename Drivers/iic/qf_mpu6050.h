#ifndef __QF_MPU6050_H__
#define __QF_MPU6050_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

#define SCL_PORT GPIOB
#define SCL_PIN GPIO_Pin_7
#define SDA_PORT GPIOB
#define SDA_PIN GPIO_Pin_8

#define SCL_OUT PBout(7)
#define SDA_OUT PBout(8)
#define SDA_IN PBin(8)

void QF_MPU6050_Init(void);
void QF_MPU6050_GetAngle(float* row, float* pitch, float* yaw);

#endif
