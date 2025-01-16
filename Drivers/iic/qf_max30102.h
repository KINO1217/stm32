#ifndef __QF_MAX30102_H__
#define __QF_MAX30102_H__

#include "qf_delay.h"
#include "qf_sys.h"
#include "stm32f10x.h"

#define MAX30102_SCL_PORT GPIOB
#define MAX30102_SCL_PIN GPIO_Pin_8
#define MAX30102_SDA_PORT GPIOB
#define MAX30102_SDA_PIN GPIO_Pin_9

#define MAXSCL_OUT PBout(8)
#define MAXSDA_OUT PBout(9)
#define MAXSDA_IN PBin(9)

#define MAX30102_WRITEADDR 0xAE
#define MAX30102_READADDR 0xAF

// register addresses
#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

void QF_MAX30102_Init(void);
void QF_MAX30102_GetData(u8* heat, u8* o2);

#endif
