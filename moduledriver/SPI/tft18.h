#ifndef __TFT18_H__
#define __TFT18_H__
#include <stm32f10x.h>

#define LCD_WIDTH 128
#define LCD_HIGHT 160
#define LCDXOFFSET 1
#define LCDYOFFSET 0

#define TFT_BLACK   0x0000
#define TFT_WHITE   0xFFFF
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_BLUE    0x001F

typedef struct __CTFT18 c_tft18;
typedef struct __CTFT18
{
    void *this;
    /**
     * @brief 清屏函数
     * @param color 用什么颜色进行清屏
     * 示例：
        tft18.clear(&tft18,TFT_WHITE);
    */
    void (*clear)(c_tft18 *this,u16 color);
    /**
     * @brief 画点函数
     * @param row 点的行坐标（1-160）
     * @param col 点的列坐标（1-128）
     * @param color 点的颜色
     * 示例：
        tft18.drawPoint(&tft18,1,1,TFT_RED);
    */
    void (*drawPoint)(c_tft18 *this,u8 row,u8 col,u16 color);
    /**
     * @brief 显示16*16大小字符函数
     * @param row 字符的行坐标（1-160）
     * @param col 字符的列坐标（1-128）
     * @param color 字符的颜色
     * @param bkColor 字符的背景颜色
     * @param fmt 要显示的字符串
     * 示例：
        tft18.showStr(&tft18,1,1,TFT_BLACK,TFT_RED,"123ABCabc");
    */
    void (*showStr)(c_tft18 *this,u8 row,u8 col,u16 color,u16 bkColor,char *fmt,...);
    /**
     * @brief 显示16*16大小中文字符函数
     * @param row 字符的行坐标（1-160）
     * @param col 字符的列坐标（1-128）
     * @param color 字符的颜色
     * @param bkColor 字符的背景颜色
     * @param str 要显示的中文字符串
     * 示例：
        tft18.showChinese(&tft18,1,1,TFT_RED,TFT_BLUE,"好好");
    */
    void (*showChinese)(c_tft18 *this,u8 row,u8 col,u16 color,u16 bkColor,char *str);
}c_tft18;

/**
 * 创建c_tft18对象
 * 示例：
    c_tft18 tft18 = {0};

    tft18 = tft18_create(GPIOA,GPIO_Pin_4,GPIOA,GPIO_Pin_3,GPIOA,GPIO_Pin_2,GPIOA,GPIO_Pin_1,GPIOA,GPIO_Pin_0);
*/
c_tft18 tft18_create(GPIO_TypeDef* SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef* SDA_Port,uint16_t SDA_Pin,GPIO_TypeDef* RES_Port,uint16_t RES_Pin,GPIO_TypeDef* DC_Port,uint16_t DC_Pin,GPIO_TypeDef* CS_Port,uint16_t CS_Pin);

#endif
