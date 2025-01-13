#ifndef __QF_TIMER_H__
#define __QF_TIMER_H__

#include "stm32f10x.h"

extern void (*QF_TIMER1_Callback)(void);
extern void (*QF_TIMER2_Callback)(void);
extern void (*QF_TIMER3_Callback)(void);
extern void (*QF_TIMER4_Callback)(void);

void QF_TIMER1_Init(u16 psc, u16 arr);
void QF_TIMER2_Init(u16 psc, u16 arr);
void QF_TIMER3_Init(u16 psc, u16 arr);
void QF_TIMER4_Init(u16 psc, u16 arr);

#endif
