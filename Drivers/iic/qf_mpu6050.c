#include "qf_mpu6050.h"
#include "math.h"

// MPU6050 REG ADDR
#define MPU6050_ADDR 0x68 // 0110 1000

#define MPU6050_SMPLRT_DIV 0x19
#define MPU6050_CONFIG 0x1A
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_CONFIG 0x1C

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_TEMP_OUT_L 0x42
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_GYRO_XOUT_L 0x44
#define MPU6050_GYRO_YOUT_H 0x45
#define MPU6050_GYRO_YOUT_L 0x46
#define MPU6050_GYRO_ZOUT_H 0x47
#define MPU6050_GYRO_ZOUT_L 0x48

#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_PWR_MGMT_2 0x6C

typedef enum {
    ACK = 0,
    NACK
} IIC_ACKEnum;

static void QF_MPU6050_IIC_Start(void);
static void QF_MPU6050_IIC_Stop(void);
static void QF_MPU6050_IIC_SendByte(uint8_t byteData);
static uint8_t QF_MPU6050_IIC_ReadByte(void);
static void QF_MPU6050_IIC_SendAck(IIC_ACKEnum ack);
static IIC_ACKEnum QF_MPU6050_IIC_GetAck(void);
static void QF_MPU6050_WriteReg(uint8_t reg, uint8_t data);
static uint8_t QF_MPU6050_ReadReg(uint8_t reg);
static void QF_MPU6050_GetData(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);

void QF_MPU6050_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT, &GPIO_InitStructure);

    SDA_OUT = 1;
    SCL_OUT = 1;
    QF_DELAY_Us(5);

    QF_MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01); // 解除睡眠
    QF_MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00); // 加速度传感器和陀螺仪都工作
    QF_MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09); // 采样率分频
    QF_MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    QF_MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18); // 陀螺仪传感器配置
    QF_MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18); // 加速度传感器配置
}

void QF_MPU6050_GetAngle(float* row, float* pitch, float* yaw)
{
    static u8 firstGet = 0;
    static float errorx = 0.0, errory = 0.0, errorz = 0.0; // 第一次采集的数据作为误差保存进行修正

    int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
    double accx = 0.0, accy = 0.0, accz = 0.0;
    double PI = 3.1415926;

    QF_MPU6050_GetData(&ax, &ay, &az, &gx, &gy, &gz); // 获取adc值

    accx = ax * 1.0 / 2048;
    accy = ay * 1.0 / 2048;
    accz = az * 1.0 / 2048;

    *row = (float)atan2(accy, accz) * 180 / PI - errorx; // 加速度解算
    *pitch = (float)-atan2(accx, sqrt(accy * accy + accz * accz)) * 180 / PI - errory;
    *yaw = 0.0 + errorz; // 暂时未解算，需要使用角速度进行解算

    if (firstGet == 0) {
        errorx = (float)*row;
        errory = (float)*pitch;
        errorz = (float)*yaw;

        firstGet = 1;
    }
}

static void QF_MPU6050_GetData(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz)
{
    uint8_t dataH, dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *ax = (dataH << 8) | dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *ay = (dataH << 8) | dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *az = (dataH << 8) | dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *gx = (dataH << 8) | dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *gy = (dataH << 8) | dataL;

    dataH = QF_MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    dataL = QF_MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *gz = (dataH << 8) | dataL;
}

static uint8_t QF_MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data = 0x00;

    QF_MPU6050_IIC_Start();
    QF_MPU6050_IIC_SendByte(MPU6050_ADDR << 1);
    QF_MPU6050_IIC_GetAck();
    QF_MPU6050_IIC_SendByte(reg);
    QF_MPU6050_IIC_GetAck();

    QF_MPU6050_IIC_Start();
    QF_MPU6050_IIC_SendByte((MPU6050_ADDR << 1) | 0x01);
    QF_MPU6050_IIC_GetAck();
    data = QF_MPU6050_IIC_ReadByte();
    QF_MPU6050_IIC_SendAck(NACK); // 连续读多个字节数据时，最后一个字节数据给非应答信号，其余给应答信号
    QF_MPU6050_IIC_Stop();

    return data;
}

static void QF_MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    QF_MPU6050_IIC_Start();
    QF_MPU6050_IIC_SendByte(MPU6050_ADDR << 1);
    QF_MPU6050_IIC_GetAck();
    QF_MPU6050_IIC_SendByte(reg);
    QF_MPU6050_IIC_GetAck();
    QF_MPU6050_IIC_SendByte(data);
    QF_MPU6050_IIC_GetAck();
    QF_MPU6050_IIC_Stop();
}

static IIC_ACKEnum QF_MPU6050_IIC_GetAck(void)
{
    IIC_ACKEnum ack = ACK;

    SDA_OUT = 1; // 释放总线

    SCL_OUT = 0;
    QF_DELAY_Us(5);
    SCL_OUT = 1;
    if (SDA_IN)
        ack = NACK;
    QF_DELAY_Us(5);
    SCL_OUT = 0;
    QF_DELAY_Us(5);

    return ack;
}

static void QF_MPU6050_IIC_SendAck(IIC_ACKEnum ack)
{
    SCL_OUT = 0;
    QF_DELAY_Us(5);
    SDA_OUT = ack;
    QF_DELAY_Us(5);
    SCL_OUT = 1;
    QF_DELAY_Us(5);
    SCL_OUT = 0;
    QF_DELAY_Us(5);
}

static uint8_t QF_MPU6050_IIC_ReadByte(void)
{
    uint8_t tempData = 0x00;

    SDA_OUT = 1; // 释放SDA总线，由上拉电阻拉为高电平

    for (uint8_t i = 0; i < 8; i++) {
        SCL_OUT = 0;
        QF_DELAY_Us(5);
        tempData <<= 1;
        SCL_OUT = 1;
        if (SDA_IN)
            tempData |= 0x01;
        QF_DELAY_Us(5);
    }

    SCL_OUT = 0;
    QF_DELAY_Us(5);

    return tempData;
}

static void QF_MPU6050_IIC_SendByte(uint8_t byteData)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        SCL_OUT = 0;
        if (byteData & 0x80)
            SDA_OUT = 1;
        else
            SDA_OUT = 0;
        QF_DELAY_Us(5);
        SCL_OUT = 1;
        QF_DELAY_Us(5);
        byteData <<= 1;
    }

    SCL_OUT = 0;
    QF_DELAY_Us(5);
}

static void QF_MPU6050_IIC_Stop(void)
{
    SDA_OUT = 0;
    SCL_OUT = 1;
    QF_DELAY_Us(5);
    SDA_OUT = 1;
    QF_DELAY_Us(5);
}

static void QF_MPU6050_IIC_Start(void)
{
    SDA_OUT = 1;
    SCL_OUT = 1;
    QF_DELAY_Us(5);
    SDA_OUT = 0;
    QF_DELAY_Us(5);
    SCL_OUT = 0;
    QF_DELAY_Us(5);
}
