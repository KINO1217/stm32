#ifndef __OLED_H__
#define __OLED_H__
#include <stm32f10x.h>
#include "my_iic.h"

typedef struct __OLED c_oled;
typedef struct __OLED
{
    /**
     * 清屏函数
     * 示例：
        oled.clear();
    */
    void (*clear)(void);
    /**
     * 开显示
     * 示例：
        oled.openDisplay();
    */
    void (*openDisplay)(void);
    /**
     * 关显示
     * 示例：
        oled.closeDisplay();
    */
    void (*closeDisplay)(void);
    /**
     * 显示字符串
     * 示例：
        oled.showString(0,0,"Hello World!");
    */
    void (*showString)(u8 row,u8 col,u8 *str,...);
    /**
     * 显示中文
     * 示例：
        oled.showChinese(0,0,"你好");
    */
    void (*showChinese)(u8 row,u8 col,u8 *str);
}c_oled;

/**
 * 创建c_oled对象
 * 示例：
    c_my_iic oled_iic = {0};
    c_oled oled = {0};

    oled_iic = iic_create(GPIOB,GPIO_Pin_6,GPIOB,GPIO_Pin_7);
    oled = oled_create(&oled_iic);
*/
c_oled oled_create(c_my_iic* iic);

#endif
