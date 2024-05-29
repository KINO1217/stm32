#ifndef __JQ8400_H__
#define __JQ8400_H__
#include <stm32f10x.h>

enum CONMODE
{
    ONELINE_MODE=0,
    UART_MODE,
};

typedef struct __CJQ8400 c_jq8400;
typedef struct __CJQ8400
{
    void *this;
    /**
     * 设置音量大小，范围vol范围0-30，0为最小，30为最大
     * 示例：
        jq8400.volume(&jq8400,10);
    */
    void (*volume)(const c_jq8400 *this,u8 vol);
    /**
     * 播放指定曲目
     * @param cnt 曲目号，单线模式下曲目命名为5个数字表示曲目号，UART模式下按照复制顺序排序
     * @param loop 是否循环，0：不循环，1：循环播放
        jq8400.play(&jq8400,1,0);
    */
    void (*play)(const c_jq8400 *this,u8 cnt,u8 loop);
    /**
     * 播放操作
     * @param mode 0：停止，1：暂停，2：继续播放
     * 示例：
        jq8400.stop(&jq8400,0);
    */
    void (*stop)(const c_jq8400 *this,u8 mode);
}c_jq8400;

/**
 * 创建c_jq8400对象
 * @param gpioPort 串口GPIO端口,UART模式下随意填写
 * @param gpioPin 串口GPIO引脚，UART模式下随意填写
 * @param uartCode 串口号，单线模式下随意填写
 * @param conMode 连接模式 0:单线模式 1:UART模式
 * @return c_jq8400对象
示例：
    c_jq8400 jq8400 = {0};

    jq8400 = jq8400_create(GPIOA,GPIO_Pin_0,MY_UART_ID_USART1,UART_MODE);
*/
c_jq8400 jq8400_create(GPIO_TypeDef *gpioPort,uint16_t gpioPin,u8 uartCode,u8 conMode);

#endif
