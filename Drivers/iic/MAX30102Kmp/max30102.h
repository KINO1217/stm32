#ifndef __MAX30102_H
#define __MAX30102_H

#include "stm32f10x.h"
#include "qf_sys.h" 

//======================================================================
// I2C 引脚定义
//======================================================================
#define MAX30102_I2C_PORT       GPIOB
#define MAX30102_SCL_PIN        GPIO_Pin_6
#define MAX30102_SDA_PIN        GPIO_Pin_7

// 位带操作宏
#define MAX30102_I2C_SCL        PBout(6) 
#define MAX30102_I2C_SDA        PBout(7) 
#define MAX30102_I2C_READ_SDA   PBin(7) 

// 中断引脚 (必须配置，否则无法通过 INT 信号同步)
#define MAX30102_INT_PIN        PBin(9)

//======================================================================
// 寄存器地址
//======================================================================
#define MAX30102_ADDR_WRITE     0xAE
#define MAX30102_ADDR_READ      0xAF

#define REG_INTR_STATUS_1       0x00
#define REG_INTR_STATUS_2       0x01
#define REG_INTR_ENABLE_1       0x02
#define REG_INTR_ENABLE_2       0x03
#define REG_FIFO_WR_PTR         0x04
#define REG_OVF_COUNTER         0x05
#define REG_FIFO_RD_PTR         0x06
#define REG_FIFO_DATA           0x07
#define REG_FIFO_CONFIG         0x08
#define REG_MODE_CONFIG         0x09
#define REG_SPO2_CONFIG         0x0A
#define REG_LED1_PA             0x0C
#define REG_LED2_PA             0x0D
#define REG_PILOT_PA            0x10
#define REG_MULTI_LED_CTRL1     0x11
#define REG_MULTI_LED_CTRL2     0x12
#define REG_TEMP_INTR           0x1F
#define REG_TEMP_FRAC           0x20
#define REG_TEMP_CONFIG         0x21
#define REG_PROX_INT_THRESH     0x30
#define REG_REV_ID              0xFE
#define REG_PART_ID             0xFF

//======================================================================
// 函数声明
//======================================================================

void MAX30102_Init(void);
void MAX30102_Reset(void);
void MAX30102_WriteReg(uint8_t reg_addr, uint8_t data);
void MAX30102_ReadReg(uint8_t reg_addr, uint8_t *data);
void MAX30102_ReadFifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);

/**
 * @brief 核心数据读取循环
 * @note  完全保留了原 Read_Data 的阻塞逻辑和缓冲区移位算法
 */
void MAX30102_ReadData(int32_t *heart_rate, int32_t *sp02);

#endif
