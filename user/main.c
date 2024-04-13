#include "driver_conf.h"

int main()
{
    vClockEnable();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级组
    
    
    while(1)
    {
        
    }
}
