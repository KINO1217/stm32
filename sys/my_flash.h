#ifndef __MY_FLASH_H__
#define __MY_FLASH_H__
#include <stm32f10x.h>

/**
 * @brief 实现FLASH数据存取操作
 * @author KINO
*/

#define LASTSECTORADDR 0x0800FC00

typedef struct __CMYFLASH
{
    /**
     * @brief 写数据
     * @param addr 起始地址
     * @param data 待写入数据
     * @param data_len 待写入数据长度
     * 示例：
        my_flash.writeData(0x0800FC00,(u8*)"123",3);
    */
    void (*writeData)(u32 addr,u8 *data,u16 data_len);
    /**
     * @brief 读数据
     * @param addr 起始地址
     * @param data 读出数据
     * @param data_len 读出数据长度
     * 示例：
        char datas[16] = {0};
        my_flash.readData(0x0800FC00, (u8*)datas, 3);
    */
    void (*readData)(u32 addr,u8 *data,u16 data_len);
}c_my_flash;

extern const c_my_flash my_flash;

#endif
