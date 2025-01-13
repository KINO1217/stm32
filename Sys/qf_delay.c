#include "qf_delay.h"

uint32_t fac_us = 0;
uint32_t fac_ms = 0;

void QF_DELAY_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 选择外部时钟9MHz
    fac_us = SystemCoreClock / 8 / 1000000; // 计算1us需要的时钟个数
    fac_ms = fac_us * 1000;
}

void QF_DELAY_Us(u32 nus)
{
    u32 temp = 0x00;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void QF_DELAY_Ms(u32 nms)
{
    u32 temp = 0x00;
    SysTick->LOAD = nms * fac_ms; // 24位寄存器，最大值0XFFFFFF=1677215，所以nms最大值1864
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}
