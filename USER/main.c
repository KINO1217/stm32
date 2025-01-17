#include "qf_conf.h"
#include "qf_delay.h"
#include "qf_driver_conf.h"

int main()
{
    // 硬件初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    QF_RCC_Conf();
    QF_DELAY_Init();

    // 模块及外设初始化

    while (1) {

        QF_DELAY_Ms(5);
    }
}
