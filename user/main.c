#include "driver_conf.h"

int main()
{
    vClockEnable(); //使能时钟
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级组
    vDelayInit();   //初始化SysTick
    
    while(1)
    {
        
    }
}
