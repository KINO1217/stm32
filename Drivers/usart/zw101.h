#ifndef __ZW101_H__
#define __ZW101_H__

#include "stm32f10x.h"
#include "qf_uart.h"
#include "qf_delay.h"

/**
    V_TOUCH TOUCH_OUT VCC TX RX GND
*/
#define ZW101_BAUD 57600

void ZW101_Init(void);
u8 ZW101_Add(u8 PageID);
u8 ZW101_Get(u16* PageID);
u8 ZW101_Delete(u16 PageID);

u8 ZW101_HandShake(void); // Œ’ ÷
u8 ZW101_GetEnrollImage(void);
u8 ZW101_GenChar(u8 BufferID);
u8 ZW101_RegModel(void);
u8 ZW101_StoreChar(u8 PageID);
u8 ZW101_GetImage(void);
u8 ZW101_Search(u8 StartPage,u8 PageNum,u16* PageID,u16* Score);
u8 ZW101_Empty(void);
u8 ZW101_ValidTempleteNum(u8* ValidN);
u8 ZW101_DeletChar(u16 PageID,u16 N);

#endif
