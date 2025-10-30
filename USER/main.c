#include "qf_conf.h"
#include "qf_delay.h"
#include "qf_driver_conf.h"

int main()
{
    // Ó²¼þ³õÊ¼»¯
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    QF_RCC_Conf();
    QF_DELAY_Init();

    while (1)
    {
        
        QF_DELAY_Ms(5);
    }
}
