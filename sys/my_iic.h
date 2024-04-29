#ifndef __MY_IIC__
#define __MY_IIC__
#include <stm32f10x.h>

/**
 * @brief 软件IIC协议驱动文件
 * @author KINO
 * @date 2024/4/19
 * 自写协议，开源于任何地方，最终解释权归KINO所有
*/

enum GPIO_MODE
{
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1
};

typedef struct __MY_IIC c_my_iic;
typedef struct __MY_IIC
{
    void *this; // 保存GPIO结构体
    /**
     * 设置SDA引脚模式，设置为输入还是输出模式
    */
    void (*sdaLineMode)(const c_my_iic *this,u8 mode);
    /**
     * 发送起始信号，用于IIC通信
    */
    void (*start)(const c_my_iic *this);
    /**
     * 发送停止信号，用于IIC通信
    */
    void (*stop)(const c_my_iic *this);
    /**
     * 接收校验位，用于IIC通信
    */
    u8 (*checkAck)(const c_my_iic *this);
    /**
     * 发送一个字节数据，用于IIC通信
    */
    void (*sendByteData)(const c_my_iic *this,u8 data);
}c_my_iic;

/**
 * 创建c_my_iic结构体对象
*/
c_my_iic iic_create(GPIO_TypeDef* SDA_GPIOx,uint16_t SDA_Pin,GPIO_TypeDef* SCL_GPIOx,uint16_t SCL_Pin);

#endif
