#include "qf_iic.h"

typedef struct __M_QF_IIC{
    GPIO_TypeDef* SCL_PORT;
    uint16_t SCL_PIN;
    GPIO_TypeDef* SDA_PORT;
    uint16_t SDA_PIN;
}m_iic;

void QF_IIC_Start(qf_iic* c_this);
void QF_IIC_Stop(qf_iic* c_this);
void QF_IIC_SendByte(qf_iic* c_this,u8 bytes);
u8 QF_IIC_ReadByte(qf_iic* c_this);
void QF_IIC_SendAck(qf_iic* c_this,u8 ack);
u8 QF_IIC_GetAck(qf_iic* c_this);

void QF_DELAY_5Us(void)
{
    u8 i = 50;

    while (i--) {
        __NOP();
    }
}

qf_iic QF_IIC_Create(GPIO_TypeDef* SCL_PORT,uint16_t SCL_PIN,GPIO_TypeDef* SDA_PORT,uint16_t SDA_PIN)
{
    qf_iic c_this; // 创建返回值
    m_iic* m_this;
    GPIO_InitTypeDef MYGPIO;
    
    // 创建内存
    m_this = (m_iic*)malloc(sizeof(m_iic));
    // 保存引脚
    m_this->SCL_PORT = SCL_PORT;
    m_this->SCL_PIN = SCL_PIN;
    m_this->SDA_PORT = SDA_PORT;
    m_this->SDA_PIN = SDA_PIN;
    
    // 初始化引脚
    MYGPIO.GPIO_Speed = GPIO_Speed_50MHz;
    MYGPIO.GPIO_Mode = GPIO_Mode_Out_OD;
    MYGPIO.GPIO_Pin = SCL_PIN;
    GPIO_Init(SCL_PORT,&MYGPIO);
    
    MYGPIO.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT,&MYGPIO);
    
    // 保存变量
    c_this.this = m_this;
    c_this.QF_IIC_Start = QF_IIC_Start;
    c_this.QF_IIC_Stop = QF_IIC_Stop;
    c_this.QF_IIC_SendByte = QF_IIC_SendByte;
    c_this.QF_IIC_ReadByte = QF_IIC_ReadByte;
    c_this.QF_IIC_SendAck = QF_IIC_SendAck;
    c_this.QF_IIC_GetAck = QF_IIC_GetAck;
    
    return c_this;
}

void QF_IIC_Start(qf_iic* c_this)
{
    m_iic* m_this = c_this->this;
    
    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_SET);
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_RESET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();
}

void QF_IIC_Stop(qf_iic* c_this)
{
    m_iic* m_this = c_this->this;
    
    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_RESET);
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_SET);
    QF_DELAY_5Us();
}

void QF_IIC_SendByte(qf_iic* c_this,u8 bytes)
{
    m_iic* m_this = c_this->this;
    u8 i = 0;

    for (i = 0; i < 8; i++) {
        GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
        if (bytes & 0x80)
            GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_SET);
        else
            GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_RESET);
        QF_DELAY_5Us();
        GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
        QF_DELAY_5Us();
        bytes <<= 1;
    }

    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();
}

u8 QF_IIC_ReadByte(qf_iic* c_this)
{
    m_iic* m_this = c_this->this;
    u8 tempData = 0x00;

    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_SET); // 释放SDA总线，由上拉电阻拉为高电平

    for (uint8_t i = 0; i < 8; i++) {
        GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
        QF_DELAY_5Us();
        tempData <<= 1;
        GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
        if (GPIO_ReadInputDataBit(m_this->SDA_PORT,m_this->SDA_PIN))
            tempData |= 0x01;
        QF_DELAY_5Us();
    }

    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();

    return tempData;
}

void QF_IIC_SendAck(qf_iic* c_this,u8 ack)
{
    m_iic* m_this = c_this->this;
    
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,(BitAction)ack);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();
}

u8 QF_IIC_GetAck(qf_iic* c_this)
{
    m_iic* m_this = c_this->this;
    u8 ack = 1;

    GPIO_WriteBit(m_this->SDA_PORT,m_this->SDA_PIN,Bit_SET); // 释放总线

    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_SET);
    ack = GPIO_ReadInputDataBit(m_this->SDA_PORT,m_this->SDA_PIN);
    QF_DELAY_5Us();
    GPIO_WriteBit(m_this->SCL_PORT,m_this->SCL_PIN,Bit_RESET);
    QF_DELAY_5Us();

    return ack;
}
