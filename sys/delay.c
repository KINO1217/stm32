#include "delay.h"

static int fac_us=0;
static int fac_ms=0;

void vDelayInit(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock/8/1000000;
    fac_ms = fac_us*1000;
}

void vDelayUs(u32 nus)
{
    u32 temp = 0;//局部变量，存在于栈中，初始值随机
    SysTick->LOAD = fac_us*nus;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do{
        temp = SysTick->CTRL;
    }while((temp&0x01) && !(temp&(1<<16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void vDelayMs(u16 nms)
{
    u8 cnt = 0;

    while((cnt+1)*1500<nms)
    {
        u32 temp = 0;
        SysTick->LOAD = (u32)fac_ms*1500;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do{
            temp = SysTick->CTRL;
        }while((temp&0x01) && !(temp&(1<<16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;

        cnt++;
    };

    if(nms%1500!=0)
    {
        u32 temp = 0;
        SysTick->LOAD = (u32)fac_ms*(nms%1500);
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do{
            temp = SysTick->CTRL;
        }while((temp&0x01) && !(temp&(1<<16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;
    }
}
