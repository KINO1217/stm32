#ifndef __W25QXX_H__
#define __W25QXX_H__

#include "qf_sys.h"
#include "stm32f10x.h"

#define W25QXX_WRITE_ENABLE 0x06
#define W25QXX_WRITE_DISABLE 0x04
#define W25QXX_READ_STATUS_REG1 0x05
#define W25QXX_PAGE_PROGRAM 0x02
#define W25QXX_SECTOR_ERASE 0x20
#define W25QXX_JEDEC_ID 0x9F
#define W25QXX_READ_DATA 0x03

/**
 * 兼容SPI模式0和模式3，上升沿发送数据，下降沿改变数据
 * 最小擦除单元扇区4KB
 * 最小写入单元页256Byte，写入不能跨页写入，读取可以跨页读取
 */
#define W25QXX_CS_PORT GPIOA
#define W25QXX_CS_PIN GPIO_Pin_0
#define W25QXX_DO_PORT GPIOA
#define W25QXX_DO_PIN GPIO_Pin_1
#define W25QXX_SLK_PORT GPIOA
#define W25QXX_SLK_PIN GPIO_Pin_2
#define W25QXX_DI_PORT GPIOA
#define W25QXX_DI_PIN GPIO_Pin_3

#define W25QXX_CS_OUT PAout(0)
#define W25QXX_DO_IN PAin(1)
#define W25QXX_SLK_OUT PAout(2)
#define W25QXX_DI_OUT PAout(3)

void QF_W25QXX_Init(void);
void QF_W25QXX_ReadID(u8* mid, u16* did);
void QF_W25QXX_WriteEnable(void);
void QF_W25QXX_WaitBusy(void);
void QF_W25QXX_PageProgram(u32 addr, u8* dat, u16 data_len);
void QF_W25QXX_SectorErase(u32 addr);
void QF_W25QXX_ReadData(u32 addr, u8* dataArray, u16 data_len);

#endif
