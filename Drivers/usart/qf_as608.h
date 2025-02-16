#ifndef __QF_AS608_H__
#define __QF_AS608_H__

#include "stm32f10x.h"

#define AS608_BAUD 57600
#define CharBuffer1 0x01
#define CharBuffer2 0x02

void QF_AS608_Init(void);
void QF_AS608_Add_FR(u16 FR_ID); // 添加指纹
void QF_AS608_Del_FR(u16 FR_ID); // 删除指纹
void QF_AS608_Get_FR(u16* FR_ID); // 获取指纹

u8 QF_AS608_GetImage(void); // 读取图像
u8 QF_AS608_GenChar(u8 CharBufferID); // 生成特征值
u8 QF_AS608_Match(void); // 比对特征值
u8 QF_AS608_RegModel(void); // 生成模板
u8 QF_AS608_StoreChar(u8 CharBufferID, u16 pageID); // 存储特征值
u8 QF_AS608_DeleteChar(u16 pageID, u16 N); // 从pageID开始删除N个模板数据
u8 QF_AS608_Empty(void); // 清空所有模板数据
u8 QF_AS608_ValidTempleteNum(u16* ValidN); // 读取有效指纹个数
u8 QF_AS608_HighSpeedSearch(u8 CharBufferID, u16 StartPage, u16 PageNum, u16* p); // 高速搜索
u8 QF_AS608_Search(u8 CharBufferID, u16 StartPage, u16 PageNum, u16* p); // 搜索指纹

#endif
