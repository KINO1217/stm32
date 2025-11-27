#ifndef __QF_OLED_H__
#define __QF_OLED_H__

#include "stm32f10x.h"

#define SDA_GPIO GPIOB
#define SDA_PIN GPIO_Pin_12
#define SCL_GPIO GPIOB
#define SCL_PIN GPIO_Pin_13

#define SDA_H GPIO_SetBits(SDA_GPIO, SDA_PIN)
#define SDA_L GPIO_ResetBits(SDA_GPIO, SDA_PIN)
#define SDA_IN GPIO_ReadInputDataBit(SDA_GPIO, SDA_PIN)
#define SCL_H GPIO_SetBits(SCL_GPIO, SCL_PIN)
#define SCL_L GPIO_ResetBits(SCL_GPIO, SCL_PIN)

void QF_OLED_Init(void);
void QF_OLED_Clear(void);
void QF_OLED_On(void);
void QF_OLED_Off(void);
void QF_OLED_Set_Pos(u8 row, u8 col);
void QF_OLED_Show_Char(u8 row, u8 col,u8 sm, u8 ch);
void QF_OLED_Show_String(u8 row, u8 col, u8 sm,u8* str, ...);
void QF_OLED_Show_OneChinese(u8 row, u8 col, u8* china);
void QF_OLED_Show_Chinese(u8 row, u8 col, u8* china);
void QF_OLED_Show_Pic(void);
void QF_DELAY_5US(void);
void QF_OLED_Gpio_Init(void);
void QF_OLED_IIC_Start(void);
void QF_OLED_IIC_Stop(void);
void QF_OLED_IIC_Send_Byte(u8 data);
u8 QF_OLED_IIC_GetAck(void);
void QF_OLED_Write_Com(u8 data, u8 com);

#endif
