#include "driver_conf.h"
#define SYS_INIT 1
int main()
{
    #if SYS_INIT
        vClockEnable();                                 // 使能时钟
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置中断优先级组
        vDelayInit();                                   // 初始化SysTick
    #endif

    while (1)
    {
    }
}
