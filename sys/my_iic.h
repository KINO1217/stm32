#ifndef __MY_IIC__
#define __MY_IIC__
#include <stm32f10x.h>

/**
 * @brief 软件模拟IIC协议驱动文件
 * @author KINO
 * @date 2024/4/19
 * 自写协议，开源于任何地方，最终解释权归KINO所有
*/

typedef struct __CIIC c_my_iic;
typedef struct __CIIC
{
    /**
     * 保存引脚信息结构体
    */
    void *this;
    /**
     * IIC协议起始信号，SCL和SDA线同为高电平时，将SDA线拉低
    */
    void (*start)(const c_my_iic *this);
    /**
     * IIC协议停止信号，SCL为高电平、SDA为低电平时，将SDA线拉高
    */
    void (*stop)(const c_my_iic *this);
    /**
     * IIC协议发送一字节数据，SCL低电平准备数据，高电平发送一位数据
    */
    void (*sendByteData)(const c_my_iic *this,u8 data);
    /**
     * IIC协议读取一字节数据，SCL低电平从机将数据放置数据线，高电平接收数据
    */
    u8 (*recvByteData)(const c_my_iic *this);
    /**
     * IIC协议接收校验位
    */
    u8 (*checkAck)(const c_my_iic *this);
    /**
     * 发送应答与不应答
    */
    void (*sendAck)(const c_my_iic *this,u8 ackBit);
}c_my_iic;

/**
 * 创建c_my_iic结构体对象
*/
c_my_iic iic_create(GPIO_TypeDef* sclPort, uint16_t sclPin, GPIO_TypeDef* sdaPort, uint16_t sdaPin);

#endif
