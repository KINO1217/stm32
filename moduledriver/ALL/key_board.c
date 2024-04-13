#include "key_board.h"
#include <stdlib.h>
#include <string.h>

static u8 totalKeyNum=0;
/*创建c_key_board对象*/
static c_key_board new = {0};
static void callback(void (*key_board_callback)(char *keyName,u8 state));
static void  (*extiCallbacks)(char *keyName,u8 state);
static void lineCallback(uint16_t GPIO_Pin);
static void delay8ms(void);
static void bindExtiLine(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

c_key_board keyboard_create(Key_Num* keyList,u8 keyCnt)
{
    /*获取按键数量*/
    totalKeyNum = keyCnt;
    
    /*申请内存*/
    new.this = malloc(sizeof(Key_Num)*totalKeyNum);
    memset(new.this,0,(sizeof(Key_Num)*totalKeyNum));
    
    /*保存变量*/
    new.this = keyList;
    new.setCallback = callback;
    
    /*初始化GPIO口*/
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    for(int i=0;i<totalKeyNum;i++){
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        if(keyList[i].currState == KEY_BOARD_STATE_ZERO)
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
        else
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStruct.GPIO_Pin = keyList[i].GPIO_Pin;
        GPIO_Init(keyList[i].GPIOx,&GPIO_InitStruct);
        
        /*绑定中断线*/
        bindExtiLine(keyList[i].GPIOx,keyList[i].GPIO_Pin);
        
        /*设置外部中断EXTI*/
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
        switch(keyList[i].GPIO_Pin){
            case GPIO_Pin_0:
                EXTI_InitStruct.EXTI_Line = EXTI_Line0;
                break;
            case GPIO_Pin_1:
                EXTI_InitStruct.EXTI_Line = EXTI_Line1;
                break;
           case GPIO_Pin_2:
                EXTI_InitStruct.EXTI_Line = EXTI_Line2;
                break;
           case GPIO_Pin_3:
                EXTI_InitStruct.EXTI_Line = EXTI_Line3;
                break;
           case GPIO_Pin_4:
                EXTI_InitStruct.EXTI_Line = EXTI_Line4;
                break;
           case GPIO_Pin_5:
                EXTI_InitStruct.EXTI_Line = EXTI_Line5;
                break;
           case GPIO_Pin_6:
                EXTI_InitStruct.EXTI_Line = EXTI_Line6;
                break;
           case GPIO_Pin_7:
                EXTI_InitStruct.EXTI_Line = EXTI_Line7;
                break;
           case GPIO_Pin_8:
                EXTI_InitStruct.EXTI_Line = EXTI_Line8;
                break;
           case GPIO_Pin_9:
                EXTI_InitStruct.EXTI_Line = EXTI_Line9;
                break;
           case GPIO_Pin_10:
                EXTI_InitStruct.EXTI_Line = EXTI_Line10;
                break;
           case GPIO_Pin_11:
                EXTI_InitStruct.EXTI_Line = EXTI_Line11;
                break;
           case GPIO_Pin_12:
                EXTI_InitStruct.EXTI_Line = EXTI_Line12;
                break;
           case GPIO_Pin_13:
                EXTI_InitStruct.EXTI_Line = EXTI_Line13;
                break;
           case GPIO_Pin_14:
                EXTI_InitStruct.EXTI_Line = EXTI_Line14;
                break;
           case GPIO_Pin_15:
                EXTI_InitStruct.EXTI_Line = EXTI_Line15;
                break;
        }
        EXTI_Init(&EXTI_InitStruct);
        
        /*设置中断优先级NVIC*/
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
        switch(keyList[i].GPIO_Pin){
            case GPIO_Pin_0:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
                break;
            case GPIO_Pin_1:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
                break;
           case GPIO_Pin_2:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
                break;
           case GPIO_Pin_3:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
                break;
           case GPIO_Pin_4:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
                break;
           case GPIO_Pin_5:
           case GPIO_Pin_6:
           case GPIO_Pin_7:
           case GPIO_Pin_8:
           case GPIO_Pin_9:
               NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
               break;
           case GPIO_Pin_10:
           case GPIO_Pin_11:
           case GPIO_Pin_12:
           case GPIO_Pin_13:
           case GPIO_Pin_14:
           case GPIO_Pin_15:
                NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
                break;
        }
        NVIC_Init(&NVIC_InitStruct);
    }
    
    return new;
}

static void callback(void (*key_board_callback)(char *keyName,u8 state))
{
    /*设置回调函数*/
    extiCallbacks = key_board_callback;
}

static void lineCallback(uint16_t GPIO_Pin)
{
    Key_Num *keyList = NULL;
    keyList = new.this;
    uint8_t retBit = 0;
    
    for(int i=0;i<totalKeyNum;i++){//遍历查找按键
        if(GPIO_Pin == keyList[i].GPIO_Pin){
            retBit = GPIO_ReadInputDataBit(keyList[i].GPIOx,keyList[i].GPIO_Pin);
            if(retBit == !keyList[i].currState){//按键消抖操作
                delay8ms();
                retBit = GPIO_ReadInputDataBit(keyList[i].GPIOx,keyList[i].GPIO_Pin);
                if(retBit == !keyList[i].currState){
                    keyList[i].currState = !keyList[i].currState;
                    extiCallbacks(keyList[i].keyName,keyList[i].currState);
                }
            }
            break;
        }
    }
}

static void delay8ms(void)
{
    for(int i=0;i<600000;i++)
        __NOP();
}

/*中断服务函数*/
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line0);//清除中断标志
        lineCallback(GPIO_Pin_0);
    }
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line1);//清除中断标志
        lineCallback(GPIO_Pin_1);
    }
}

void EXTI2_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line2) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line2);//清除中断标志
        lineCallback(GPIO_Pin_2);
    }
}

void EXTI3_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line3) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line3);//清除中断标志
        lineCallback(GPIO_Pin_3);
    }
}

void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line4);//清除中断标志
        lineCallback(GPIO_Pin_4);
    }
}

void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line9) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line9);//清除中断标志
        lineCallback(GPIO_Pin_9);
    }
    else if(EXTI_GetITStatus(EXTI_Line8) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line8);//清除中断标志
        lineCallback(GPIO_Pin_8);
    }
    else if(EXTI_GetITStatus(EXTI_Line7) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line7);//清除中断标志
        lineCallback(GPIO_Pin_7);
    }
    else if(EXTI_GetITStatus(EXTI_Line6) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line6);//清除中断标志
        lineCallback(GPIO_Pin_6);
    }
    else if(EXTI_GetITStatus(EXTI_Line5) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志
        lineCallback(GPIO_Pin_5);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line15) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line15);//清除中断标志
        lineCallback(GPIO_Pin_15);
    }
    else if(EXTI_GetITStatus(EXTI_Line14) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line14);//清除中断标志
        lineCallback(GPIO_Pin_14);
    }
    else if(EXTI_GetITStatus(EXTI_Line13) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line13);//清除中断标志
        lineCallback(GPIO_Pin_13);
    }
    else if(EXTI_GetITStatus(EXTI_Line12) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line12);//清除中断标志
        lineCallback(GPIO_Pin_12);
    }
    else if(EXTI_GetITStatus(EXTI_Line11) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line11);//清除中断标志
        lineCallback(GPIO_Pin_11);
    }
    else if(EXTI_GetITStatus(EXTI_Line10) == SET)//读取中断线状态
    {
        EXTI_ClearITPendingBit(EXTI_Line10);//清除中断标志
        lineCallback(GPIO_Pin_10);
    }
}

static void bindExtiLine(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
    switch(GPIO_Pin){
        case GPIO_Pin_0:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);
            break;
        case GPIO_Pin_1:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource1);
            break;
       case GPIO_Pin_2:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource2);
            break;
       case GPIO_Pin_3:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource3);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource3);
            break;
       case GPIO_Pin_4:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
            break;
       case GPIO_Pin_5:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
            break;
       case GPIO_Pin_6:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);
            break;
       case GPIO_Pin_7:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource7);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource7);
            break;
       case GPIO_Pin_8:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource8);
            break;
       case GPIO_Pin_9:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource9);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource9);
            break;
       case GPIO_Pin_10:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource10);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource10);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource10);
            break;
       case GPIO_Pin_11:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource11);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource11);
            break;
       case GPIO_Pin_12:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource12);
            break;
       case GPIO_Pin_13:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource13);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
            break;
       case GPIO_Pin_14:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource14);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);
            break;
       case GPIO_Pin_15:
            if(GPIOx == GPIOA)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
            else if(GPIOx == GPIOB)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);
            else if(GPIOx == GPIOC)
                GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
            break;
    }
}
