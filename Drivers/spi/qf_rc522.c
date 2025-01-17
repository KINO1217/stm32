#include "qf_rc522.h"

static void QF_SPI_SendByte(uint8_t data);
static uint8_t QF_SPI_ReadByte(void);
static void QF_WriteRawRC(uint8_t addr, uint8_t data);
static uint8_t QF_ReadRawRC(uint8_t addr);
static char QF_ConfigWorkISOType(u8 type);
static void QF_ClearBitMask(u8 reg, u8 mask);
static void QF_SetBitMask(u8 reg, u8 mask);
static void QF_PcdAntennaOn(void);
static void QF_PcdAntennaOff(void);
static char QF_PcdComRC522(u8 ucCommand, u8* pInData, u8 ucInLenByte, u8* pOutData, u32* pOutLenBit);
static void QF_CalulateCRC(u8* pIn, u8 len, u8* pOut);

void QF_RC522_Init(void)
{
    // 初始化引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = RST_PIN;
    GPIO_Init(RST_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = MOSI_PIN;
    GPIO_Init(MOSI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SCK_PIN;
    GPIO_Init(SCK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 数据接收引脚
    GPIO_InitStructure.GPIO_Pin = MISO_PIN;
    GPIO_Init(MISO_PORT, &GPIO_InitStructure);

    // 初始化引脚电平,SPI空闲状态
    SCK_OUT = 1;
    SDA_OUT = 1;

    // 复位RC522
    RST_OUT = 1;
    QF_DELAY_Us(5);
    RST_OUT = 0;
    QF_DELAY_Us(5);
    RST_OUT = 1;
    QF_DELAY_Us(5);

    QF_WriteRawRC(CommandReg, PCD_RESETPHASE); // 写RC632寄存器，复位
    while (QF_ReadRawRC(CommandReg) & 0x10)
        ;

    QF_WriteRawRC(ModeReg, 0x3D); // 定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
    QF_WriteRawRC(TReloadRegL, 30); // 16位定时器低位
    QF_WriteRawRC(TReloadRegH, 0); // 16位定时器高位
    QF_WriteRawRC(TModeReg, 0x8D); // 定义内部定时器的设置
    QF_WriteRawRC(TPrescalerReg, 0x3E); // 设置定时器分频系数
    QF_WriteRawRC(TxAutoReg, 0x40); // 调制发送信号为100%ASK

    QF_PcdAntennaOff(); // 关闭天线
    QF_DELAY_Ms(2); // 延时2毫秒
    QF_PcdAntennaOn(); // 开启天线

    // 设置工作模式
    QF_ConfigWorkISOType('A');
}

/**
 * 写RC522寄存器
 * @param addr 寄存器地址
 * @param data 要写入的数据
 */
static void QF_WriteRawRC(uint8_t addr, uint8_t data)
{
    u8 tempAddr = (addr << 1) & 0x7E; // 最后一位保留不使用，最高位为0表示写入

    SDA_OUT = 0; // 开始通信
    QF_SPI_SendByte(tempAddr); // 写入地址
    QF_SPI_SendByte(data); // 写入数据
    SDA_OUT = 1; // 结束通信
}

/**
 * 读RC522寄存器
 * @param addr 寄存器地址
 * @return 读取到的数据
 */
static uint8_t QF_ReadRawRC(uint8_t addr)
{
    uint8_t retData = 0x00;
    u8 tempAddr = ((addr << 1) & 0x7E) | 0x80; // 最后一位保留不使用，最高位为1表示读取

    SDA_OUT = 0; // 开始通信
    QF_SPI_SendByte(tempAddr); // 写入地址
    retData = QF_SPI_ReadByte(); // 读取数据
    SDA_OUT = 1; // 结束通信

    return retData;
}

/**
 * 清除寄存器指定位
 * @param reg 寄存器地址
 * @param mask 要清除的位
 */
static void QF_ClearBitMask(u8 reg, u8 mask)
{
    u8 temp;

    temp = QF_ReadRawRC(reg);
    QF_WriteRawRC(reg, temp & (~mask)); // clear bit mask
}

/**
 * @Brief   设置寄存器指定位
 * @Para    Reg  寄存器地址
 * @Para    Mask
 */
static void QF_SetBitMask(u8 reg, u8 mask)
{
    u8 temp;

    temp = QF_ReadRawRC(reg);
    QF_WriteRawRC(reg, temp | mask); // set bit mask
}

/**
 * 配置RC522工作模式
 * @param type ISO14443_A or ISO14443_B
 * @return MI_OK or MI_ERR
 */
static char QF_ConfigWorkISOType(u8 type)
{
    if (type == 'A') // ISO14443_A
    {
        QF_ClearBitMask(Status2Reg, 0x08); // 清RC522寄存器位
        QF_WriteRawRC(ModeReg, 0x3D); // 3F//CRC初始值0x6363
        QF_WriteRawRC(RxSelReg, 0x86); // 84
        QF_WriteRawRC(RFCfgReg, 0x7F); // 4F  //调整卡的感应距离//RxGain = 48dB调节卡感应距离
        QF_WriteRawRC(TReloadRegL, 30); // tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        QF_WriteRawRC(TReloadRegH, 0);
        QF_WriteRawRC(TModeReg, 0x8D);
        QF_WriteRawRC(TPrescalerReg, 0x3E);
        QF_DELAY_Us(20);
        QF_PcdAntennaOn(); // 开启天线
    } else
        return MI_ERR; // 失败，返回1
    return MI_OK; // 成功返回0
}

/*
 * 描述  ：通过RC522和ISO14443卡通讯
 * 输入  ：ucCommand，RC522命令字
 *         pInData，通过RC522发送到卡片的数据
 *         ucInLenByte，发送数据的字节长度
 *         pOutData，接收到的卡片返回数据
 *         pOutLenBit，返回数据的位长度
 * 返回  : 状态值
 *         = MI_OK，成功
 * 调用  ：内部调用
 */
static char QF_PcdComRC522(u8 ucCommand, u8* pInData, u8 ucInLenByte, u8* pOutData, u32* pOutLenBit)
{
    char cStatus = MI_ERR;
    u8 ucIrqEn = 0x00;
    u8 ucWaitFor = 0x00;
    u8 ucLastBits;
    u8 ucN;
    u32 ul;

    switch (ucCommand) {
    case PCD_AUTHENT: // Mifare认证
        ucIrqEn = 0x12; // 允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10; // 认证寻卡等待时候 查询空闲中断标志位
        break;
    case PCD_TRANSCEIVE: // 接收发送 发送接收
        ucIrqEn = 0x77; // 允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30; // 寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;
    default:
        break;
    }

    QF_WriteRawRC(ComIEnReg, ucIrqEn | 0x80); // IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
    QF_ClearBitMask(ComIrqReg, 0x80); // Set1该位清零时，CommIRqReg的屏蔽位清零
    QF_WriteRawRC(CommandReg, PCD_IDLE); // 写空闲命令
    QF_SetBitMask(FIFOLevelReg, 0x80); // 置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除

    for (ul = 0; ul < ucInLenByte; ul++)
        QF_WriteRawRC(FIFODataReg, pInData[ul]); // 写数据进FIFOdata

    QF_WriteRawRC(CommandReg, ucCommand); // 写命令

    if (ucCommand == PCD_TRANSCEIVE)
        QF_SetBitMask(BitFramingReg, 0x80); // StartSend置位启动数据发送 该位与收发命令使用时才有效

    ul = 1000; // 根据时钟频率调整，操作M1卡最大等待时间25ms

    do // 认证 与寻卡等待时间
    {
        ucN = QF_ReadRawRC(ComIrqReg); // 查询事件中断
        ul--;
    } while ((ul != 0) && (!(ucN & 0x01)) && (!(ucN & ucWaitFor))); // 退出条件i=0,定时器中断，与写空闲命令

    QF_ClearBitMask(BitFramingReg, 0x80); // 清理允许StartSend位

    if (ul != 0) {
        if (!((QF_ReadRawRC(ErrorReg) & 0x1B))) // 读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;

            if (ucN & ucIrqEn & 0x01) // 是否发生定时器中断
                cStatus = MI_NOTAGERR;

            if (ucCommand == PCD_TRANSCEIVE) {
                ucN = QF_ReadRawRC(FIFOLevelReg); // 读FIFO中保存的字节数

                ucLastBits = QF_ReadRawRC(ControlReg) & 0x07; // 最后接收到得字节的有效位数

                if (ucLastBits)
                    *pOutLenBit = (ucN - 1) * 8 + ucLastBits; // N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
                else
                    *pOutLenBit = ucN * 8; // 最后接收到的字节整个字节有效

                if (ucN == 0)
                    ucN = 1;

                if (ucN > MAXRLEN)
                    ucN = MAXRLEN;

                for (ul = 0; ul < ucN; ul++)
                    pOutData[ul] = QF_ReadRawRC(FIFODataReg);
            }
        } else
            cStatus = MI_ERR;
    }

    QF_SetBitMask(ControlReg, 0x80); // stop timer now
    QF_WriteRawRC(CommandReg, PCD_IDLE);

    return cStatus;
}

static void QF_PcdAntennaOn(void)
{
    u8 uc = 0x00;

    uc = QF_ReadRawRC(TxControlReg);
    if ((uc & 0x03) != 0x03)
        QF_SetBitMask(TxControlReg, 0x03);
}

static void QF_PcdAntennaOff(void)
{
    QF_ClearBitMask(TxControlReg, 0x03);
}

u8 QF_RC522_IS_Connected(void)
{
    return (QF_ReadRawRC(VersionReg) == 0x92);
}

/*
功    能: 寻卡
参数说明: req_code[IN]:寻卡方式
                0x52   = 寻感应区内所有符合14443A标准的卡
                0x26   = 寻未进入休眠状态的卡
                pTagType[OUT]:卡片类型代码
                0x4400 = Mifare_UltraLight
                0x0400 = Mifare_One(S50)
                0x0200 = Mifare_One(S70)
                0x0800 = Mifare_Pro(X)
                0x4403 = Mifare_DESFire
返 回 值: 成功返回MI_OK
*/
char QF_PcdRequest(u8 req_code, u8* pTagType)
{
    char status;
    u32 unLen;
    u8 ucComMF522Buf[MAXRLEN]; // MAXRLEN  18

    QF_ClearBitMask(Status2Reg, 0x08); // 清RC522寄存器位,/接收数据命令
    QF_WriteRawRC(BitFramingReg, 0x07); // 写RC632寄存器
    QF_SetBitMask(TxControlReg, 0x03); // 置RC522寄存器位

    ucComMF522Buf[0] = req_code; // 寻卡方式

    status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen); // 通过RC522和ISO14443卡通讯

    if ((status == MI_OK) && (unLen == 0x10)) {
        *pTagType = ucComMF522Buf[0];
        *(pTagType + 1) = ucComMF522Buf[1];
    } else {
        status = MI_ERR;
    }
    return status;
}

/*
 * 描述  ：防冲撞
 * 输入  ：pSnr，卡片序列号，4字节
 * 返回  : 状态值
 *         = MI_OK，成功
 */
char QF_PcdAnticoll(u8* pSnr)
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf[MAXRLEN];
    u32 ulLen;

    QF_ClearBitMask(Status2Reg, 0x08); // 清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    QF_WriteRawRC(BitFramingReg, 0x00); // 清理寄存器 停止收发
    QF_ClearBitMask(CollReg, 0x80); // 清ValuesAfterColl所有接收的位在冲突后被清除

    ucComMF522Buf[0] = 0x93; // 卡片防冲突命令
    ucComMF522Buf[1] = 0x20;

    cStatus = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &ulLen); // 与卡片通信

    if (cStatus == MI_OK) // 通信成功
    {
        for (uc = 0; uc < 4; uc++) {
            *(pSnr + uc) = ucComMF522Buf[uc]; // 读出UID
            ucSnr_check ^= ucComMF522Buf[uc];
        }

        if (ucSnr_check != ucComMF522Buf[uc])
            cStatus = MI_ERR;
    }

    QF_SetBitMask(CollReg, 0x80);

    return cStatus;
}

/**
 * 功    能：选定卡片
 * 参数说明：pSnr[IN]:卡片ID，4字节
 * 返    回：成功返回MI_OK
 */
char QF_PcdSelectTag(u8* pSnr)
{
    char status;
    u8 i;
    u32 unLen;
    u8 ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;

    for (i = 0; i < 4; i++) {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6] ^= *(pSnr + i);
    }

    QF_CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]); // 用MF522计算CRC16函数，校验数据
    QF_ClearBitMask(Status2Reg, 0x08); // 清RC522寄存器位

    status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);
    if ((status == MI_OK) && (unLen == 0x18))
        status = MI_OK;
    else
        status = MI_ERR;

    return status;
}

/**
 * 功    能：验证卡片密码
 * 参数说明：auth_mode[IN]: 密码验证模式
 *                0x60 = 验证A密钥
 *                0x61 = 验证B密钥
 *          addr[IN]：块地址0-63
 *          pKey[IN]：扇区密码，6字节
 *          pSnr[IN]：卡片序列号，4字节
 * 返    回：成功返回MI_OK
 */
char QF_PcdAuthPasswd(u8 auth_mode, u8 addr, u8* pKey, u8* pSnr)
{
    char status;
    u32 unLen;
    u8 ucComMF522Buf[MAXRLEN]; // MAXRLEN  18(数组的大小)

    // 验证模式+块地址+扇区密码+卡序列号
    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6); // 拷贝，复制
    memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = QF_PcdComRC522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if ((status != MI_OK) || (!(QF_ReadRawRC(Status2Reg) & 0x08)))
        status = MI_ERR;

    return status;
}

/**
 * 功    能：读取M1卡一块数据
 * 参数说明：
 *      addr：块地址0-63
 *      p   ：读出的块数据，16字节
 * 返    回：成功返回MI_OK
 */
char QF_PcdReadData(u8 addr, u8* p)
{
    char status;
    u32 unLen;
    u8 i, ucComMF522Buf[MAXRLEN]; // 18

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    QF_CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status == MI_OK && (unLen == 0x90))) {
        for (i = 0; i < 16; i++)
            *(p + i) = ucComMF522Buf[i];
    } else {
        status = MI_ERR;
    }

    return status;
}

/**
 * 功    能：写数据到M1卡指定块
 * 参数说明：addr：块地址0-63
 *          p   ：向块写入的数据，16字节
 * 返    回：成功返回MI_OK
 */
char QF_PcdWriteData(u8 addr, u8* p)
{
    char status;
    u32 unLen;
    u8 i, ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE; // 0xA0 //写块
    ucComMF522Buf[1] = addr; // 块地址
    QF_CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }

    if (status == MI_OK) {
        for (i = 0; i < 16; i++) // 向FIFO写16Byte数据
        {
            ucComMF522Buf[i] = *(p + i);
        }
        QF_CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);

        status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }

    return status;
}

#if 0

/**
 * 功    能：命令卡片进入休眠状态
 * 返    回：成功返回MI_OK
 */
char QF_PcdHalt(void)
{
    u8 status;
    u32 unLen;
    u8 ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    QF_CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

    status = QF_PcdComRC522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);

    return status;
}

/**
 * 功能:选卡读取卡存储器容量
 * 输入参数:serNum 传入卡ID，4字节
 * 返 回 值:成功返回卡容量
 */
u8 QF_PcdGetSize(u8* serNum)
{
    u8 i;
    u8 status;
    u8 size;
    u32 recvBits;
    u8 buffer[9];

    buffer[0] = PICC_ANTICOLL1; // 防撞码1
    buffer[1] = 0x70;
    buffer[6] = 0x00;

    for (i = 0; i < 4; i++) {
        buffer[i + 2] = *(serNum + i); // buffer[2]-buffer[5]为卡序列号
        buffer[6] ^= *(serNum + i); // 卡校验码
    }

    QF_CalulateCRC(buffer, 7, &buffer[7]); // buffer[7]-buffer[8]为RCR校验码
    QF_ClearBitMask(Status2Reg, 0x08);

    status = QF_PcdComRC522(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

    if ((status == MI_OK) && (recvBits == 0x18))
        size = buffer[0] / 8; // KB
    else
        size = 0;

    return size;
}

#endif

/**
 * 功    能：用MF522计算CRC16函数
 * 参    数：
 *          *pIn ：要读数CRC的数据
 *          len：数据长度
 *          *pOut：计算的CRC结果
 */
static void QF_CalulateCRC(u8* pIn, u8 len, u8* pOut)
{
    u8 i, n;

    QF_ClearBitMask(DivIrqReg, 0x04); // CRCIrq = 0
    QF_WriteRawRC(CommandReg, PCD_IDLE);
    QF_SetBitMask(FIFOLevelReg, 0x80); // 清FIFO指针

    // 向FIFO中写入数据
    for (i = 0; i < len; i++) {
        QF_WriteRawRC(FIFODataReg, *(pIn + i)); // 开始RCR计算
    }

    QF_WriteRawRC(CommandReg, PCD_CALCCRC); // 等待CRC计算完成

    i = 0xFF;
    do {
        n = QF_ReadRawRC(DivIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x04)); // CRCIrq = 1

    // 读取CRC计算结果
    pOut[0] = QF_ReadRawRC(CRCResultRegL);
    pOut[1] = QF_ReadRawRC(CRCResultRegM);
}

static void QF_SPI_SendByte(uint8_t data)
{
    for (u8 i = 0; i < 8; i++) {
        SCK_OUT = 0; // 准备数据
        if (data & 0x80)
            MOSI_OUT = 1;
        else
            MOSI_OUT = 0;
        SCK_OUT = 1; // 将数据发送出去
        data <<= 1;
    }

    SCK_OUT = 1; // 空闲状态
}

static uint8_t QF_SPI_ReadByte(void)
{
    uint8_t data = 0x00;

    for (u8 i = 0; i < 8; i++) {
        data <<= 1;
        SCK_OUT = 0; // 等待从机准备数据
        SCK_OUT = 1; // 获取从机交换过来的数据
        if (MISO_IN)
            data |= 0x01;
    }

    SCK_OUT = 1; // 空闲状态

    return data;
}
