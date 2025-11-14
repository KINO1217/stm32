#include "qf_ir_send.h"

static void QF_SendHy(u16 cnt);
static void QF_NotSendHy(u16 cnt);
static void QF_Nec0(void);
static void QF_Nec1(void);

void QF_IRSEND_Init(void)
{
    GPIO_InitTypeDef ENStruct;
    ENStruct.GPIO_Speed = GPIO_Speed_50MHz;
    ENStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    ENStruct.GPIO_Pin = ENPIN;
    GPIO_Init(ENPORT,&ENStruct);
}

void QF_IRSEND_SendData(u16 addrCode,u8 datas)
{
    u8 i=0;
    u8 temp = 0x00;
    
    // 引导码
    QF_SendHy(342);
    QF_NotSendHy(171);
    
    // 发送地址和反码
    for(i=0;i<16;i++){
        if(addrCode & 0x0001)
            QF_Nec1();
        else
            QF_Nec0();
        addrCode >>= 1;
    }
    
    // 发送数据
    temp = datas;
    for(i=0;i<8;i++){
        if(temp & 0x01)
            QF_Nec1();
        else
            QF_Nec0();
        temp >>= 1;
    }
    
    // 发送数据反码
    temp = ~datas;
    for(i=0;i<8;i++){
        if(temp & 0x01)
            QF_Nec1();
        else
            QF_Nec0();
        temp >>= 1;
    }
    
    // 结束码
    QF_Nec0();
}

static void QF_Nec1(void)
{
    QF_SendHy(21);
    QF_NotSendHy(64);
}

static void QF_Nec0(void)
{
    QF_SendHy(21);
    QF_NotSendHy(21);
}

static void QF_NotSendHy(u16 cnt)
{
    while(cnt--){
        ENOUT = 0;
        QF_DELAY_Us(26);
    }
}

static void QF_SendHy(u16 cnt)
{
    while(cnt--){
        ENOUT = 1;
        QF_DELAY_Us(8);
        ENOUT = 0;
        QF_DELAY_Us(18);
    }
}
