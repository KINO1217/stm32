#include "hc_sr04.h"
#include <stdlib.h>
#include <string.h>
#include "delay.h"

typedef struct __MHCSR04
{
    GPIO_TypeDef* GPIO_Trig;
    uint16_t Trig_Pin;
    GPIO_TypeDef* GPIO_Echo;
    uint16_t Echo_Pin;
}m_hcsr04;

void get(c_hcsr04* this,float* dis,float temp);

c_hcsr04 hcsr04_create(GPIO_TypeDef* GPIO_Trig,uint16_t Trig_Pin,GPIO_TypeDef* GPIO_Echo,uint16_t Echo_Pin)
{
    /*初始化GPIO口*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = Trig_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_Trig,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = Echo_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_Echo,&GPIO_InitStruct);

    /**创建m_hcsr04对象，申请空间赋值*/
    c_hcsr04 new = {0};
    new.this = (m_hcsr04*)malloc(sizeof(m_hcsr04));
    memset(new.this,0,sizeof(new.this));
    ((m_hcsr04*)new.this)->GPIO_Trig = GPIO_Trig;
    ((m_hcsr04*)new.this)->Trig_Pin = Trig_Pin;
    ((m_hcsr04*)new.this)->GPIO_Echo = GPIO_Echo;
    ((m_hcsr04*)new.this)->Echo_Pin = Echo_Pin;
    new.get = get;

    /**初始化TIM2，用于计数*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

    TIM_Cmd(TIM2,ENABLE); //使能/开启定时器2

    return new;
}

void get(c_hcsr04* this,float* dis,float temp)
{
    m_hcsr04 *m_this = (m_hcsr04*)this->this;

    /**发送触发信号*/
    GPIO_SetBits(m_this->GPIO_Trig,m_this->Trig_Pin);
    vDelayUs(10);
    GPIO_ResetBits(m_this->GPIO_Trig,m_this->Trig_Pin);

    /**接收回波时间长度*/
    while(GPIO_ReadInputDataBit(m_this->GPIO_Echo,m_this->Echo_Pin) == RESET);
    TIM_SetCounter(TIM2,0);
    while(GPIO_ReadInputDataBit(m_this->GPIO_Echo,m_this->Echo_Pin) == SET);
    u16 time = TIM_GetCounter(TIM2);

    //计算障碍物距离
    *dis = (float)time*(334.1+0.6*temp) / 20000.0;
    vDelayMs(5);
}
