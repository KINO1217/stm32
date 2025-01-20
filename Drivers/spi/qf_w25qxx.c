#include "qf_w25qxx.h"

u8 QF_W25QXX_SwapData(u8 data);

void QF_W25QXX_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = W25QXX_CS_PIN;
    GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = W25QXX_SLK_PIN;
    GPIO_Init(W25QXX_SLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = W25QXX_DI_PIN;
    GPIO_Init(W25QXX_DI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = W25QXX_DO_PIN;
    GPIO_Init(W25QXX_DO_PORT, &GPIO_InitStructure);

    // 空闲状态，SPI模式0
    W25QXX_CS_OUT = 1;
    W25QXX_SLK_OUT = 0;
}

void QF_W25QXX_ReadData(u32 addr, u8* dataArray, u16 data_len)
{
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_READ_DATA);
    QF_W25QXX_SwapData(addr >> 16);
    QF_W25QXX_SwapData(addr >> 8);
    QF_W25QXX_SwapData(addr);
    for (u16 i = 0; i < data_len; i++) {
        dataArray[i] = QF_W25QXX_SwapData(0xFF);
    }
    W25QXX_CS_OUT = 1;
}

void QF_W25QXX_SectorErase(u32 addr)
{
    QF_W25QXX_WriteEnable();
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_SECTOR_ERASE);
    QF_W25QXX_SwapData(addr >> 16);
    QF_W25QXX_SwapData(addr >> 8);
    QF_W25QXX_SwapData(addr);
    W25QXX_CS_OUT = 1;
    QF_W25QXX_WaitBusy();
}

void QF_W25QXX_PageProgram(u32 addr, u8* dat, u16 data_len)
{
    QF_W25QXX_WriteEnable();
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_PAGE_PROGRAM);
    QF_W25QXX_SwapData(addr >> 16);
    QF_W25QXX_SwapData(addr >> 8);
    QF_W25QXX_SwapData(addr);
    for (u16 i = 0; i < data_len; i++) {
        QF_W25QXX_SwapData(dat[i]);
    }
    W25QXX_CS_OUT = 1;
    QF_W25QXX_WaitBusy();
}

void QF_W25QXX_WaitBusy(void)
{
    u32 timeout = 0;
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_READ_STATUS_REG1);
    while ((QF_W25QXX_SwapData(0xFF) & 0x01) && timeout < 100000) { // 最低位是忙标志位，1表示忙，0表示空闲
        timeout++;
    }
    W25QXX_CS_OUT = 1;
}

void QF_W25QXX_WriteEnable(void)
{
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_WRITE_ENABLE);
    W25QXX_CS_OUT = 1;
}

void QF_W25QXX_ReadID(u8* mid, u16* did)
{
    W25QXX_CS_OUT = 0; // 开始通信
    QF_W25QXX_SwapData(W25QXX_JEDEC_ID);
    *mid = QF_W25QXX_SwapData(0xFF);
    *did = QF_W25QXX_SwapData(0xFF);
    *did <<= 8;
    *did |= QF_W25QXX_SwapData(0xFF);
    W25QXX_CS_OUT = 1;
}

u8 QF_W25QXX_SwapData(u8 data)
{
    u8 retDat = 0x00;

    for (u8 i = 0; i < 8; i++) {
        retDat <<= 1;
        W25QXX_SLK_OUT = 0;
        if (data & 0x80)
            W25QXX_DI_OUT = 1;
        else
            W25QXX_DI_OUT = 0;
        W25QXX_SLK_OUT = 1;
        if (W25QXX_DO_IN)
            retDat |= 0x01;
        data <<= 1;
    }

    W25QXX_SLK_OUT = 0;

    return retDat;
}
