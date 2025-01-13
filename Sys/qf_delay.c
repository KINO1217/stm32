#include "qf_delay.h"

uint32_t fac_us = 0;
uint32_t fac_ms = 0;

void QF_DELAY_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // ѡ���ⲿʱ��9MHz
    fac_us = SystemCoreClock / 8 / 1000000; // ����1us��Ҫ��ʱ�Ӹ���
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
    SysTick->LOAD = nms * fac_ms; // 24λ�Ĵ��������ֵ0XFFFFFF=1677215������nms���ֵ1864
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}
