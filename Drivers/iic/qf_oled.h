#ifndef __QF_OLED_H
#define __QF_OLED_H

#include "stm32f10x.h"

/* OLED基本参数 */
#define QF_OLED_WIDTH          128
#define QF_OLED_HEIGHT         64
#define QF_OLED_PAGE_NUM       8

/* OLED IIC写地址 */
#define QF_OLED_IIC_ADDR       0x78

/* 引脚定义 */
#define QF_OLED_SDA_PORT       GPIOB
#define QF_OLED_SDA_PIN        GPIO_Pin_12

#define QF_OLED_SCL_PORT       GPIOB
#define QF_OLED_SCL_PIN        GPIO_Pin_13

/* printf缓冲区长度 */
#define QF_OLED_PRINTF_BUF_LEN 64

/* OLED显存 */
extern uint8_t QF_OLED_GRAM[QF_OLED_PAGE_NUM][QF_OLED_WIDTH];

/* 基础接口 */
void QF_OLED_Init(void);
void QF_OLED_Clear(void);
void QF_OLED_Fill(uint8_t data);
void QF_OLED_Refresh(void);

/* 显示控制 */
void QF_OLED_DisplayOn(void);
void QF_OLED_DisplayOff(void);

/* 绘图接口 */
void QF_OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void QF_OLED_ClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/* 文本接口 */
void QF_OLED_ShowChar8x16(uint8_t x, uint8_t y, char chr);
void QF_OLED_ShowString8x16(uint8_t x, uint8_t y, const char *str);

/* 数字接口 */
void QF_OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
void QF_OLED_ShowHexNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

/* Printf增强接口 */
void QF_OLED_Printf8x16(uint8_t x, uint8_t y, const char *format, ...);

#endif
