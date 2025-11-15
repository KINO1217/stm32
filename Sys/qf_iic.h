#ifndef __QF_IIC_H__
#define __QF_IIC_H__

#include "stm32f10x.h"
#include "stdlib.h"

typedef struct __QF_IIC qf_iic;
typedef struct __QF_IIC{
    void* this;
    void (*QF_IIC_Start)(qf_iic* c_this);
    void (*QF_IIC_Stop)(qf_iic* c_this);
    void (*QF_IIC_SendByte)(qf_iic* c_this,u8 bytes);
    u8 (*QF_IIC_ReadByte)(qf_iic* c_this);
    void (*QF_IIC_SendAck)(qf_iic* c_this,u8 ack);
    u8 (*QF_IIC_GetAck)(qf_iic* c_this);
}qf_iic;

qf_iic QF_IIC_Create(GPIO_TypeDef* SCL_PORT,uint16_t SCL_PIN,GPIO_TypeDef* SDA_PORT,uint16_t SDA_PIN);

#endif
