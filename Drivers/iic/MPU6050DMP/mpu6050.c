#include "mpu6050.h"

void MPU6050_IIC_Start(void);
void MPU6050_IIC_Stop(void);
void MPU6050_IIC_SendByte(uint8_t byteData);
uint8_t MPU6050_IIC_ReadByte(void);
void MPU6050_IIC_SendAck(uint8_t ack);
uint8_t MPU6050_IIC_GetAck(void);

void QF_DELAY_5US(void)
{
    u8 i = 50;

    while (i--) {
        __NOP();
    }
}

uint8_t MPU6050_IIC_GetAck(void)
{
    uint8_t ack = 1;

    MPU6050_SDA_OUT = 1; // 释放总线

    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();
    MPU6050_SCL_OUT = 1;
    if(!MPU6050_SDA_IN)
        ack = 0;
    QF_DELAY_5US();
    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();

    return ack;
}

void MPU6050_IIC_SendAck(uint8_t ack)
{
    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();
    MPU6050_SDA_OUT = ack;
    QF_DELAY_5US();
    MPU6050_SCL_OUT = 1;
    QF_DELAY_5US();
    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();
}

uint8_t MPU6050_IIC_ReadByte(void)
{
    uint8_t tempData = 0x00;

    MPU6050_SDA_OUT = 1; // 释放SDA总线，由上拉电阻拉为高电平

    for (uint8_t i = 0; i < 8; i++) {
        MPU6050_SCL_OUT = 0;
        QF_DELAY_5US();
        tempData <<= 1;
        MPU6050_SCL_OUT = 1;
        if (MPU6050_SDA_IN)
            tempData |= 0x01;
        QF_DELAY_5US();
    }

    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();

    return tempData;
}

void MPU6050_IIC_SendByte(uint8_t byteData)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        MPU6050_SCL_OUT = 0;
        if (byteData & 0x80)
            MPU6050_SDA_OUT = 1;
        else
            MPU6050_SDA_OUT = 0;
        QF_DELAY_5US();
        MPU6050_SCL_OUT = 1;
        QF_DELAY_5US();
        byteData <<= 1;
    }

    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();
}

void MPU6050_IIC_Stop(void)
{
    MPU6050_SDA_OUT = 0;
    MPU6050_SCL_OUT = 1;
    QF_DELAY_5US();
    MPU6050_SDA_OUT = 1;
    QF_DELAY_5US();
}

void MPU6050_IIC_Start(void)
{
    MPU6050_SDA_OUT = 1;
    MPU6050_SCL_OUT = 1;
    QF_DELAY_5US();
    MPU6050_SDA_OUT = 0;
    QF_DELAY_5US();
    MPU6050_SCL_OUT = 0;
    QF_DELAY_5US();
}

u8 MPU6050_Write(u8 addr, u8 reg, u8 len, u8* buf)//返回值 0：读成功  -1：读失败
{ 
    unsigned char i;
    addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
    MPU6050_IIC_Start();              //启动总线
    MPU6050_IIC_SendByte(addr);      //发送器件地址
    MPU6050_IIC_GetAck();
    MPU6050_IIC_SendByte(reg);       //发送器件子地址
    MPU6050_IIC_GetAck();

    for(i=0;i<len;i++){
        MPU6050_IIC_SendByte(*buf++);  //发送数据
        MPU6050_IIC_GetAck();
    }
    MPU6050_IIC_Stop();               //结束总线

    return 0;
}

u8 MPU6050_Read(u8 addr, u8 reg, u8 len, u8* buf)//返回值 0：读成功  -1：读失败
{
    unsigned char i;
    addr=addr<<1;                     //注意dma库地址不包含最后一位，需要移位
    MPU6050_IIC_Start();              //启动总线
    MPU6050_IIC_SendByte(addr);      //发送器件地址
    MPU6050_IIC_GetAck();
    MPU6050_IIC_SendByte(reg);       //发送器件子地址
    MPU6050_IIC_GetAck();

    MPU6050_IIC_Start();              //重新启动总线
    MPU6050_IIC_SendByte(addr+1);
    MPU6050_IIC_GetAck();
    for(i=0;i<len-1;i++){
        *buf++=MPU6050_IIC_ReadByte();//发送数据
        MPU6050_IIC_SendAck(0);
    }
    *buf=MPU6050_IIC_ReadByte();
    MPU6050_IIC_SendAck(1);
    MPU6050_IIC_Stop();               //结束总线

    return 0;
}

void MPU6050_Write_Reg(u8 reg, u8 dat)
{
   MPU6050_Write(MPU_ADDR,reg,1,&dat);
}

u8 MPU6050_Read_Reg(u8 reg)
{
    u8 dat;
    MPU6050_Read(MPU_ADDR,reg,1,&dat);
    return dat;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//其他,设置失败 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
    MPU6050_Write_Reg(GYRO_CONFIG,fsr<<3);//设置陀螺仪满量程范围  
    return 0;
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//其他,设置失败
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
    MPU6050_Write_Reg(ACCEL_CONFIG,fsr<<3);//设置加速度传感器满量程范围  
    return 0;
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//其他,设置失败 
u8 MPU_Set_LPF(u16 lpf)
{
    u8 data=0;
    if(lpf>=188)data=1;
    else if(lpf>=98)data=2;
    else if(lpf>=42)data=3;
    else if(lpf>=20)data=4;
    else if(lpf>=10)data=5;
    else data=6; 
    MPU6050_Write_Reg(MPU_CFG_REG,data);//设置数字低通滤波器  
    return 0;
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
u8 MPU_Set_Rate(u16 rate)
{
    u8 data;
    if(rate>1000)rate=1000;
    if(rate<4)rate=4;
    data=1000/rate-1;
    MPU6050_Write_Reg(MPU_SAMPLE_RATE_REG,data);    //设置数字低通滤波器
    return MPU_Set_LPF(rate/2); //自动设置LPF为采样率的一半
}

void MPU6050_Init(void)
{ 
    u8 res; 
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MPU6050_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MPU6050_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MPU6050_SDA_PIN;
    GPIO_Init(MPU6050_SDA_PORT, &GPIO_InitStructure);
    
    MPU6050_SDA_OUT = 1;
    MPU6050_SCL_OUT = 1;
    
    MPU6050_Write_Reg(PWR_MGMT_1,0X80);         //复位MPU6050
    QF_DELAY_Ms(100);
    MPU6050_Write_Reg(PWR_MGMT_1,0X00);         //唤醒MPU6050 
    MPU_Set_Gyro_Fsr(3);                        //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(0);                       //加速度传感器,±2g
    MPU_Set_Rate(200);                          //设置采样率50Hz
    MPU6050_Write_Reg(MPU_INT_EN_REG,0X00);     //关闭所有中断
    MPU6050_Write_Reg(MPU_USER_CTRL_REG,0X00);  //I2C主模式关闭
    MPU6050_Write_Reg(MPU_FIFO_EN_REG,0X00);    //关闭FIFO
    MPU6050_Write_Reg(MPU_INTBP_CFG_REG,0X80);  //INT引脚低电平有效
    res=MPU6050_Read_Reg(MPU_DEVICE_ID_REG); 
    if(res==MPU_ADDR)                           //器件ID正确
    {
        MPU6050_Write_Reg(PWR_MGMT_1,0X01);     //设置CLKSEL,PLL X轴为参考
        MPU6050_Write_Reg(PWR_MGMT_2,0X00);     //加速度与陀螺仪都工作
        MPU_Set_Rate(100);                      //设置采样率为50Hz
    }
}
 
//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
    float temp;
    MPU6050_Read(MPU_ADDR,TEMP_OUT_H,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
    res=MPU6050_Read(MPU_ADDR,GYRO_XOUT_H,6,buf);
    if(res==0)
    {
        *gx=((u16)buf[0]<<8)|buf[1];  
        *gy=((u16)buf[2]<<8)|buf[3];  
        *gz=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
    res=MPU6050_Read(MPU_ADDR,ACCEL_XOUT_H,6,buf);
    if(res==0)
    {
        *ax=((u16)buf[0]<<8)|buf[1];  
        *ay=((u16)buf[2]<<8)|buf[3];  
        *az=((u16)buf[4]<<8)|buf[5];
    }
    return res;
}
