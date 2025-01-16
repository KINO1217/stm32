#ifndef __QF_IR_RECV_H__
#define __QF_IR_RECV_H__

#include "qf_delay.h"
#include "stm32f10x.h"

/**
 * 38KHz载波
 * 红外遥控数据格式：
 *     引导码9ms低电平和4.5ms高电平
 *     控制码、地址反码、地址码和地址反码，低电平先接收，0.56ms低电平（有载波）和0.56ms高电平（无载波）表示比特0
 *     0.56ms低电平（有载波）和1.685ms高电平（无载波）表示比特1
 */

#define IR_RECVIER_GPIO GPIOA
#define IR_RECVIER_PIN GPIO_Pin_0
#define IR_IN GPIO_ReadInputDataBit(IR_RECVIER_GPIO, IR_RECVIER_PIN)

#define ENABLE_INT {NVIC->ISER[EXTI0_IRQn >> 0x05] = (uint32_t)0x01 << (EXTI0_IRQn & (uint8_t)0x1F);}
#define DISABLE_INT {NVIC->ICER[EXTI0_IRQn >> 0x05] = (uint32_t)0x01 << (EXTI0_IRQn & (uint8_t)0x1F);}

void QF_IRRECV_Init(void);
extern void (*QF_IRRECV_Callback)(u8* data);

#endif
