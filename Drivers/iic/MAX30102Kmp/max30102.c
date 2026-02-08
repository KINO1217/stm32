#include "max30102.h"
#include "algorithm.h"
#include "qf_delay.h" 
#include <stdio.h>

//======================================================================
// 全局变量
//======================================================================
// 注意：BUFFER_SIZE 应在 algorithm.h 中定义 (500)
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 500 
#endif

uint32_t aun_ir_buffer[BUFFER_SIZE];  
uint32_t aun_red_buffer[BUFFER_SIZE]; 
int32_t  n_ir_buffer_length;          
int32_t  n_sp02;                      
int8_t   ch_spo2_valid;               
int32_t  n_heart_rate;                
int8_t   ch_hr_valid;                 

//======================================================================
// 私有 I2C 底层函数 (原 xiic.c 内容融合)
//======================================================================

static void MAX30102_I2C_Delay(void) {
    QF_DELAY_Us(2); 
}

static void MAX30102_I2C_SDA_Out(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MAX30102_I2C_PORT, &GPIO_InitStructure);
}

static void MAX30102_I2C_SDA_In(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(MAX30102_I2C_PORT, &GPIO_InitStructure);
}

static void MAX30102_I2C_Start(void) {
    MAX30102_I2C_SDA_Out();
    MAX30102_I2C_SDA = 1;
    MAX30102_I2C_SCL = 1;
    MAX30102_I2C_Delay();
    MAX30102_I2C_SDA = 0;
    MAX30102_I2C_Delay();
    MAX30102_I2C_SCL = 0;
}

static void MAX30102_I2C_Stop(void) {
    MAX30102_I2C_SDA_Out();
    MAX30102_I2C_SCL = 0;
    MAX30102_I2C_SDA = 0;
    MAX30102_I2C_Delay();
    MAX30102_I2C_SCL = 1;
    MAX30102_I2C_SDA = 1;
    MAX30102_I2C_Delay();
}

static void MAX30102_I2C_SendByte(uint8_t txd) {
    uint8_t t;
    MAX30102_I2C_SDA_Out();
    MAX30102_I2C_SCL = 0;
    for (t = 0; t < 8; t++) {
        MAX30102_I2C_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        MAX30102_I2C_Delay();
        MAX30102_I2C_SCL = 1;
        MAX30102_I2C_Delay();
        MAX30102_I2C_SCL = 0;
        MAX30102_I2C_Delay();
    }
}

static uint8_t MAX30102_I2C_WaitAck(void) {
    uint8_t ucErrTime = 0;
    MAX30102_I2C_SDA_In();
    MAX30102_I2C_SDA = 1;
    MAX30102_I2C_Delay();
    MAX30102_I2C_SCL = 1;
    MAX30102_I2C_Delay();
    while (MAX30102_I2C_READ_SDA) {
        ucErrTime++;
        if (ucErrTime > 250) {
            MAX30102_I2C_Stop();
            return 1;
        }
    }
    MAX30102_I2C_SCL = 0;
    return 0;
}

static uint8_t MAX30102_I2C_ReadByte(unsigned char ack) {
    unsigned char i, receive = 0;
    MAX30102_I2C_SDA_In();
    for (i = 0; i < 8; i++) {
        MAX30102_I2C_SCL = 0;
        MAX30102_I2C_Delay();
        MAX30102_I2C_SCL = 1;
        receive <<= 1;
        if (MAX30102_I2C_READ_SDA) receive++;
        MAX30102_I2C_Delay();
    }
    MAX30102_I2C_SCL = 0;
    MAX30102_I2C_SDA_Out();
    if (!ack) MAX30102_I2C_SDA = 1; 
    else      MAX30102_I2C_SDA = 0; 
    MAX30102_I2C_Delay();
    MAX30102_I2C_SCL = 1;
    MAX30102_I2C_Delay();
    MAX30102_I2C_SCL = 0;
    return receive;
}

static void MAX30102_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Pin = MAX30102_SCL_PIN | MAX30102_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MAX30102_I2C_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(MAX30102_I2C_PORT, MAX30102_SCL_PIN | MAX30102_SDA_PIN);
}

//======================================================================
// MAX30102 驱动功能函数
//======================================================================

void MAX30102_WriteReg(uint8_t reg_addr, uint8_t data) {
    MAX30102_I2C_Start();
    MAX30102_I2C_SendByte(MAX30102_ADDR_WRITE);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_SendByte(reg_addr);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_SendByte(data);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_Stop();
}

void MAX30102_ReadReg(uint8_t reg_addr, uint8_t *data) {
    MAX30102_I2C_Start();
    MAX30102_I2C_SendByte(MAX30102_ADDR_WRITE);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_SendByte(reg_addr);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_Start();
    MAX30102_I2C_SendByte(MAX30102_ADDR_READ);
    MAX30102_I2C_WaitAck();
    *data = MAX30102_I2C_ReadByte(0); 
    MAX30102_I2C_Stop();
}

void MAX30102_ReadFifo(uint32_t *pun_red_led, uint32_t *pun_ir_led) {
    uint32_t un_temp;
    uint8_t temp_reg;
    uint8_t ach_i2c_data[6];
    
    *pun_red_led = 0;
    *pun_ir_led = 0;

    // 清除状态位，保证中断引脚正确复位
    MAX30102_ReadReg(REG_INTR_STATUS_1, &temp_reg);
    MAX30102_ReadReg(REG_INTR_STATUS_2, &temp_reg);

    MAX30102_I2C_Start();
    MAX30102_I2C_SendByte(MAX30102_ADDR_WRITE);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_SendByte(REG_FIFO_DATA);
    MAX30102_I2C_WaitAck();
    MAX30102_I2C_Start();
    MAX30102_I2C_SendByte(MAX30102_ADDR_READ);
    MAX30102_I2C_WaitAck();
    
    ach_i2c_data[0] = MAX30102_I2C_ReadByte(1); 
    ach_i2c_data[1] = MAX30102_I2C_ReadByte(1);
    ach_i2c_data[2] = MAX30102_I2C_ReadByte(1);
    ach_i2c_data[3] = MAX30102_I2C_ReadByte(1);
    ach_i2c_data[4] = MAX30102_I2C_ReadByte(1);
    ach_i2c_data[5] = MAX30102_I2C_ReadByte(0); 
    
    MAX30102_I2C_Stop();

    un_temp = (unsigned char)ach_i2c_data[0];
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[1];
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[2];
    *pun_red_led += un_temp;

    un_temp = (unsigned char)ach_i2c_data[3];
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[4];
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = (unsigned char)ach_i2c_data[5];
    *pun_ir_led += un_temp;

    *pun_red_led &= 0x03FFFF; 
    *pun_ir_led &= 0x03FFFF;  
}

void MAX30102_Reset(void) {
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x40);
    QF_DELAY_Ms(10);
}

void MAX30102_Init(void) {
    uint32_t i;
    MAX30102_GPIO_Init();
    MAX30102_Reset();
    
    MAX30102_WriteReg(REG_INTR_ENABLE_1, 0xc0); 
    MAX30102_WriteReg(REG_INTR_ENABLE_2, 0x00);
    MAX30102_WriteReg(REG_FIFO_WR_PTR, 0x00);
    MAX30102_WriteReg(REG_OVF_COUNTER, 0x00);
    MAX30102_WriteReg(REG_FIFO_RD_PTR, 0x00);
    MAX30102_WriteReg(REG_FIFO_CONFIG, 0x0f);   
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x03);   
    MAX30102_WriteReg(REG_SPO2_CONFIG, 0x27);   
    MAX30102_WriteReg(REG_LED1_PA, 0x24);       
    MAX30102_WriteReg(REG_LED2_PA, 0x24);       
    MAX30102_WriteReg(REG_PILOT_PA, 0x7f);      

    n_ir_buffer_length = BUFFER_SIZE; 
    
    // 初始化时填充前 500 个数据
    for (i = 0; i < n_ir_buffer_length; i++) {
        while(MAX30102_INT_PIN == 1); // 等待中断
        MAX30102_ReadFifo(&aun_red_buffer[i], &aun_ir_buffer[i]);
    }
    
    MAX30102_Algorithm_Process(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

// 恢复原始逻辑的 Loop 函数
void MAX30102_ReadData(int32_t *heart_rate, int32_t *sp02) {
    int i;
    // 原始逻辑：这里是一个 while(1) 循环
    // 它通过移动缓冲区和补充100个新数据来维持算法的连续性
    while(1)
    {
        // 1. 移动缓冲区：将后400个数据移到最前面
        for(i = 100; i < BUFFER_SIZE; i++)
        {
            aun_red_buffer[i - 100] = aun_red_buffer[i];
            aun_ir_buffer[i - 100] = aun_ir_buffer[i];
        }
        
        // 2. 采集新数据：填充后100个位置
        // 这个 for 循环是严格阻塞的，必须采集满100次
        for(i = 400; i < BUFFER_SIZE; i++)
        {
            while(MAX30102_INT_PIN == 1); // 严格等待硬件中断信号
            
            MAX30102_ReadFifo(&aun_red_buffer[i], &aun_ir_buffer[i]);
            
            // 原代码在此处有计算 brightness 的逻辑，但不影响心率计算的核心数据
            // 为保证时序一致，这里仅保留数据读取，去除了 printf 和纯展示用的计算
        }

        // 3. 执行算法
        MAX30102_Algorithm_Process(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

        // 4. 判断有效性并返回
        // 只有当数据有效且在合理范围内时才更新并跳出
        if((ch_hr_valid == 1) && (ch_spo2_valid == 1) && (n_heart_rate < 120) && (n_sp02 > 80)) 
        {
            *heart_rate = n_heart_rate;
            *sp02 = n_sp02;
            break; // 成功获取，跳出循环
        }
        else
        {
            *heart_rate = 0;
            *sp02 = 0;
            break; // 获取失败，跳出循环（原代码逻辑如此）
                   // 如果你希望它一直阻塞直到读到有效数据，请把这个 else break 注释掉
        }
    }
}
