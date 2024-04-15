#include "switch.h"
#include <stdlib.h>
#include <string.h>

typedef struct __MSWITCH{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
}m_switch;

static void set(const c_switch* this,u8 state);

c_switch switch_create(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
    /*初始化GPIO口*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出模式
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//高速模式
    GPIO_Init(GPIOx,&GPIO_InitStruct);
    
    /*创建c_switch对象，设置结构体变量*/
    c_switch new = {0};
    new.this = malloc(sizeof(m_switch));//申请内存空间
    memset(new.this,0,sizeof(m_switch));
    ((m_switch*)new.this)->GPIOx = GPIOx;
    ((m_switch*)new.this)->GPIO_Pin = GPIO_Pin;
    new.set = set;
    
    return new;
}

static void set(const c_switch* this,u8 state)
{
    /*创建m_switch对象，获取GPIO*/
    m_switch* m_this = NULL;
    m_this = this->this;
    
    //设置端口电平
    if(state == SWITCH_HIGHT)
    {
        GPIO_SetBits(m_this->GPIOx,m_this->GPIO_Pin);
    }
    else if(state == SWITCH_LOW)
    {
        GPIO_ResetBits(m_this->GPIOx,m_this->GPIO_Pin);
    }
}
