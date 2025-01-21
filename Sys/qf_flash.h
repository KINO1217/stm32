#ifndef __QF_FLASH_H__
#define __QF_FLASH_H__

#include "stm32f10x.h"

void QF_FLASH_ErasePage(u32 addr);
void QF_FLASH_Write(u32 addr, u8* data, u16 data_len);
void QF_FLASH_Read(u32 addr, u8* data, u16 data_len);

#endif
