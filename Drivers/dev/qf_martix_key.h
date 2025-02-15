#ifndef __QF_MARTIX_KEY_H__
#define __QF_MARTIX_KEY_H__

#define C4_PORT GPIOA
#define C4_PIN GPIO_Pin_7
#define C3_PORT GPIOA
#define C3_PIN GPIO_Pin_6
#define C2_PORT GPIOA
#define C2_PIN GPIO_Pin_5
#define C1_PORT GPIOA
#define C1_PIN GPIO_Pin_4

#define R1_PORT GPIOA
#define R1_PIN GPIO_Pin_3
#define R2_PORT GPIOA
#define R2_PIN GPIO_Pin_2
#define R3_PORT GPIOA
#define R3_PIN GPIO_Pin_1
#define R4_PORT GPIOA
#define R4_PIN GPIO_Pin_0

#define C4_Out(x) GPIO_WriteBit(C4_PORT, C4_PIN, (BitAction)x)
#define C3_Out(x) GPIO_WriteBit(C3_PORT, C3_PIN, (BitAction)x)
#define C2_Out(x) GPIO_WriteBit(C2_PORT, C2_PIN, (BitAction)x)
#define C1_Out(x) GPIO_WriteBit(C1_PORT, C1_PIN, (BitAction)x)
#define R1_Out(x) GPIO_WriteBit(R1_PORT, R1_PIN, (BitAction)x)
#define R2_Out(x) GPIO_WriteBit(R2_PORT, R2_PIN, (BitAction)x)
#define R3_Out(x) GPIO_WriteBit(R3_PORT, R3_PIN, (BitAction)x)
#define R4_Out(x) GPIO_WriteBit(R4_PORT, R4_PIN, (BitAction)x)

#define C4_In GPIO_ReadInputDataBit(C4_PORT, C4_PIN)
#define C3_In GPIO_ReadInputDataBit(C3_PORT, C3_PIN)
#define C2_In GPIO_ReadInputDataBit(C2_PORT, C2_PIN)
#define C1_In GPIO_ReadInputDataBit(C1_PORT, C1_PIN)
#define R1_In GPIO_ReadInputDataBit(R1_PORT, R1_PIN)
#define R2_In GPIO_ReadInputDataBit(R2_PORT, R2_PIN)
#define R3_In GPIO_ReadInputDataBit(R3_PORT, R3_PIN)
#define R4_In GPIO_ReadInputDataBit(R4_PORT, R4_PIN)

#include "stm32f10x.h"

void QF_MARTIX_KEY_Init(void);
u8 QF_MARTIX_KEY_Scan(u8 mode);

#endif
