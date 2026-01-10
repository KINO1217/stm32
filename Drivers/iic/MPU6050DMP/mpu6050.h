#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "qf_sys.h"
#include "qf_delay.h"

#define MPU_SAMPLE_RATE_REG     0X19    //采样频率分频器
#define MPU_CFG_REG             0X1A    //配置寄存器
#define GYRO_CONFIG             0x1B    //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define ACCEL_CONFIG            0x1C    //加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define ACCEL_XOUT_H            0x3B
#define ACCEL_XOUT_L            0x3C
#define ACCEL_YOUT_H            0x3D
#define ACCEL_YOUT_L            0x3E
#define ACCEL_ZOUT_H            0x3F

#define MPU_FIFO_EN_REG         0X23    //FIFO使能寄存器
#define MPU_I2CMST_STA_REG      0X36    //IIC主机状态寄存器
#define MPU_INTBP_CFG_REG       0X37    //中断/旁路设置寄存器
#define MPU_INT_EN_REG          0X38    //中断使能寄存器
#define MPU_INT_STA_REG         0X3A    //中断状态寄存器
#define MPU_USER_CTRL_REG       0X6A    //用户控制寄存器

#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48
#define PWR_MGMT_1      0x6B 
#define PWR_MGMT_2      0x6C
#define MPU_DEVICE_ID_REG       0x75
#define MPU_ADDR        0x68    //IIC地址寄存器0x68

#define MPU6050_SDA_PORT GPIOB
#define MPU6050_SDA_PIN GPIO_Pin_11
#define MPU6050_SCL_PORT GPIOB
#define MPU6050_SCL_PIN GPIO_Pin_10

#define MPU6050_SCL_OUT PBout(10)
#define MPU6050_SDA_OUT PBout(11)
#define MPU6050_SDA_IN PBin(11)

u8 MPU6050_Write(u8 addr, u8 reg, u8 len, u8* buf);//返回值 0：读成功  -1：读失败
u8 MPU6050_Read(u8 addr, u8 reg, u8 len, u8* buf);//返回值 0：读成功  -1：读失败
void MPU6050_Write_Reg(u8 reg, u8 dat);
u8 MPU6050_Read_Reg(u8 reg);

void MPU6050_Init(void);
short MPU_Get_Temperature(void);
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz);
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az);

#endif
