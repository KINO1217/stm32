#ifndef __QF_OLED_H__
#define __QF_OLED_H__

#include "stm32f10x.h"

#define SDA_GPIO GPIOB
#define SDA_PIN GPIO_Pin_12
#define SCL_GPIO GPIOB
#define SCL_PIN GPIO_Pin_13

#define OLED_SDA_H SDA_GPIO->BSRR = SDA_PIN
#define OLED_SDA_L SDA_GPIO->BRR = SDA_PIN
#define OLED_SDA_IN (SDA_GPIO->IDR & SDA_PIN) ? 1 : 0
#define OLED_SCL_H SCL_GPIO->BSRR = SCL_PIN
#define OLED_SCL_L SCL_GPIO->BRR = SCL_PIN

void QF_OLED_Init(void);
void QF_OLED_Clear(void);
void QF_OLED_On(void);
void QF_OLED_Off(void);
void QF_OLED_Set_Pos(u8 row, u8 col);
void QF_OLED_Show_Char(u8 row, u8 col, char ch);
void QF_OLED_Show_String(u8 row, u8 col, char *str, ...);
void QF_OLED_Show_SChinese(u8 row, u8 col, char *china);
void QF_OLED_Show_Chinese(u8 row, u8 col, char *china);

void QF_DELAY_5US(void);
void QF_OLED_Gpio_Init(void);
void QF_OLED_IIC_Start(void);
void QF_OLED_IIC_Stop(void);
void QF_OLED_IIC_Send_Byte(u8 data);
u8 QF_OLED_IIC_GetAck(void);
void QF_OLED_Write_Com(u8 data, u8 com);

#endif
