#include "qf_conf.h"
#include "qf_delay.h"
#include "qf_driver_conf.h"

int main()
{
    // Ӳ����ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    QF_RCC_Conf();
    QF_DELAY_Init();

    // ģ�鼰�����ʼ��

    while (1) {
        QF_DELAY_Ms(5);
    }
}
