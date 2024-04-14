#ifndef __DELAY_H__
#define __DELAY_H__
#include <stm32f10x.h>

void vDelayInit(void);
void vDelayUs(u32 nus);
void vDelayMs(u16 nms);

#endif
