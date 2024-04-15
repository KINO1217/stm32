#ifndef __KEY_BOADR_H__
#include <stm32f10x.h>

/***按键高低电平状态***/
enum __KEY_BOARD_STATE
{
    KEY_BOARD_STATE_ZERO = 0,
    KEY_BOARD_STATE_ONE
};

/***********按键属性结构体**********/
typedef struct __KEY
{
    char keyName[20]; // 按键名称
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    u8 currState; // 按键当前状态
} Key_Num;

typedef struct __KEYBOARD c_key_board;
typedef struct __KEYBOARD
{
    void *this; // 保存按键列表
    /**
     * 设置回调函数
     * 示例：
        void keyCallback(char* keyName,u8 state)
        {
            return;
        }

        key_board.setCallback(keyCallback);
    */
    void (*setCallback)(void (*key_board_callback)(char *keyName, u8 state));
} c_key_board;
/*
 * 创建c_key_board对象
 * 示例：
    c_key_board key_board = {0};

    Key_Num keyList[]={
        {"KEY1",GPIOA,GPIO_Pin_5,KEY_BOARD_STATE_ZERO},
        {"KEY2",GPIOA,GPIO_Pin_6,KEY_BOARD_STATE_ZERO}
    };
    key_board = keyboard_create(keyList,sizeof(keyList)/sizeof(Key_Num));
*/
c_key_board keyboard_create(Key_Num *keyList, u8 keyCnt);
#endif
