#include "qf_max30102.h"

static void QF_MAX30102_IIC_Start(void);
static void QF_MAX30102_IIC_Stop(void);
static void QF_MAX30102_IIC_SendByte(uint8_t data);
static uint8_t QF_MAX30102_IIC_ReadByte(void);
static uint8_t QF_MAX30102_IIC_CheckAck(void);
static void QF_MAX30102_IIC_SendAck(uint8_t ack);
static void QF_MAX30102_WriteReg(uint8_t reg, uint8_t value);
static uint8_t QF_MAX30102_ReadReg(uint8_t reg);
static void QF_MAX30102_ReadFIFO(uint32_t data[]);
static int8_t QF_MAX30102_FIFODataSize(void);

static u8 mheat = 0;
static u8 mo2 = 0;

void QF_MAX30102_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MAX30102_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MAX30102_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_PIN;
    GPIO_Init(MAX30102_SDA_PORT, &GPIO_InitStructure);

    MAXSDA_OUT = 1;
    MAXSCL_OUT = 1;
    QF_DELAY_Us(5);

    QF_MAX30102_WriteReg(REG_MODE_CONFIG, 0x40); // 模块复位
    QF_DELAY_Ms(10);

    QF_MAX30102_WriteReg(REG_INTR_ENABLE_1, 0xC0); // 使能中断
    QF_MAX30102_WriteReg(REG_INTR_ENABLE_2, 0x00);
    QF_MAX30102_WriteReg(REG_FIFO_WR_PTR, 0x00); // 样本缓冲区地址都写0，一个样本6字节，3字节心率数据，3字节血氧数据
    QF_MAX30102_WriteReg(REG_OVF_COUNTER, 0x00); // OVF_COUNTER[4:0]
    QF_MAX30102_WriteReg(REG_FIFO_RD_PTR, 0x00); // 读指针都写0
    QF_MAX30102_WriteReg(REG_FIFO_CONFIG, 0x5F); // 每相邻的四个样本取一个平均值，选择集满15个数据就置起缓冲区满了的标志
    QF_MAX30102_WriteReg(REG_MODE_CONFIG, 0x03); // 选择血氧饱和度 红外和红光模式
    QF_MAX30102_WriteReg(REG_SPO2_CONFIG, 0x27); // 量程 = 4096nA
    QF_MAX30102_WriteReg(REG_LED1_PA, 0x27);
    QF_MAX30102_WriteReg(REG_LED2_PA, 0x27);
    QF_MAX30102_WriteReg(REG_PILOT_PA, 0x7F);
}

void QF_MAX30102_GetData(u8* heat, u8* o2)
{
    uint32_t redIrDat[2];
    uint8_t nums = 0;
    uint32_t redData = 0;
    uint32_t irData = 0;
    uint8_t loopCnt = 0;

    while(loopCnt<10){
        nums = QF_MAX30102_FIFODataSize();
        for (u8 i = 0; i < nums; i++) {
            QF_MAX30102_ReadFIFO(redIrDat);
            redData += redIrDat[0];
            irData += redIrDat[1];
        }
        loopCnt+=nums;
    }

    if (nums != 0) {
        redData /= loopCnt;
        irData /= loopCnt;
    }

    if (redData > 50000 && irData > 50000) {
        mheat = redData / 1450;
        mo2 = irData / 1350;
    } else {
        mheat = 0;
        mo2 = 0;
    }

    redData = 0;
    irData = 0;

    *heat = mheat;
    *o2 = mo2;

    QF_DELAY_Ms(50);
}

static void QF_MAX30102_ReadFIFO(uint32_t data[])
{
    char buf[6];

    QF_MAX30102_IIC_Start();
    QF_MAX30102_IIC_SendByte(MAX30102_WRITEADDR);
    QF_MAX30102_IIC_CheckAck();
    QF_MAX30102_IIC_SendByte(REG_FIFO_DATA);
    QF_MAX30102_IIC_CheckAck();

    QF_MAX30102_IIC_Start();
    QF_MAX30102_IIC_SendByte(MAX30102_READADDR);
    QF_MAX30102_IIC_CheckAck();

    buf[0] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(0);
    buf[1] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(0);
    buf[2] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(0);
    buf[3] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(0);
    buf[4] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(0);
    buf[5] = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(1);

    QF_MAX30102_IIC_Stop();

    data[0] = (((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2])) & 0x3FFFF; // red
    data[1] = (((uint32_t)buf[3] << 16) | ((uint32_t)buf[4] << 8) | ((uint32_t)buf[5])) & 0x3FFFF; // ir
}

static int8_t QF_MAX30102_FIFODataSize(void)
{
    int8_t size = 0;
    uint8_t endAddr, startAddr;

    endAddr = QF_MAX30102_ReadReg(REG_FIFO_WR_PTR);
    startAddr = QF_MAX30102_ReadReg(REG_FIFO_RD_PTR);

    if (endAddr != startAddr) {
        size = endAddr - startAddr;
        if (size < 0)
            size += 32;
    }

    return size;
}

static void QF_MAX30102_WriteReg(uint8_t reg, uint8_t value)
{
    QF_MAX30102_IIC_Start();
    QF_MAX30102_IIC_SendByte(MAX30102_WRITEADDR);
    QF_MAX30102_IIC_CheckAck();
    QF_MAX30102_IIC_SendByte(reg);
    QF_MAX30102_IIC_CheckAck();
    QF_MAX30102_IIC_SendByte(value);
    QF_MAX30102_IIC_CheckAck();
    QF_MAX30102_IIC_Stop();
}

static uint8_t QF_MAX30102_ReadReg(uint8_t reg)
{
    uint8_t value = 0x00;

    QF_MAX30102_IIC_Start();
    QF_MAX30102_IIC_SendByte(MAX30102_WRITEADDR);
    QF_MAX30102_IIC_CheckAck();
    QF_MAX30102_IIC_SendByte(reg);
    QF_MAX30102_IIC_CheckAck();

    QF_MAX30102_IIC_Start();
    QF_MAX30102_IIC_SendByte(MAX30102_READADDR);
    QF_MAX30102_IIC_CheckAck();
    value = QF_MAX30102_IIC_ReadByte();
    QF_MAX30102_IIC_SendAck(1); // 发送响应表示还有数据需要读
    QF_MAX30102_IIC_Stop();

    return value;
}

static void QF_MAX30102_IIC_Start(void)
{
    MAXSDA_OUT = 1;
    MAXSCL_OUT = 1;
    QF_DELAY_Us(5);
    MAXSDA_OUT = 0;
    QF_DELAY_Us(5);
    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);
}

static void QF_MAX30102_IIC_Stop(void)
{
    MAXSDA_OUT = 0;
    MAXSCL_OUT = 1;
    QF_DELAY_Us(5);
    MAXSDA_OUT = 1;
    QF_DELAY_Us(5);
}

static void QF_MAX30102_IIC_SendByte(uint8_t data)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        MAXSCL_OUT = 0;
        if (data & 0x80)
            MAXSDA_OUT = 1;
        else
            MAXSDA_OUT = 0;
        QF_DELAY_Us(5);
        MAXSCL_OUT = 1;
        QF_DELAY_Us(5);
        data <<= 1;
    }

    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);
}

static uint8_t QF_MAX30102_IIC_ReadByte(void)
{
    uint8_t i = 0;
    uint8_t data = 0;

    MAXSDA_OUT = 1;

    for (i = 0; i < 8; i++) {
        data <<= 1;
        MAXSCL_OUT = 0;
        QF_DELAY_Us(5);
        MAXSCL_OUT = 1;
        if (MAXSDA_IN)
            data |= 0x01;
        QF_DELAY_Us(5);
    }

    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);

    return data;
}

static uint8_t QF_MAX30102_IIC_CheckAck(void)
{
    uint8_t ack = 0;

    MAXSDA_OUT = 1;

    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);
    MAXSCL_OUT = 1;
    if (MAXSDA_IN)
        ack = 1;
    QF_DELAY_Us(5);

    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);

    return ack;
}

static void QF_MAX30102_IIC_SendAck(uint8_t ack)
{
    MAXSCL_OUT = 0;
    if (ack)
        MAXSDA_OUT = 1;
    else
        MAXSDA_OUT = 0;
    QF_DELAY_Us(5);
    MAXSCL_OUT = 1;
    QF_DELAY_Us(5);

    MAXSCL_OUT = 0;
    QF_DELAY_Us(5);
}
