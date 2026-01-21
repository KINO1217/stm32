#include "zw101.h"

char ZW101_RECV_BUF[20] = {0};
u8 ZW101_RECV_FLAG = 0;

void ZW101_Callback(u8* data, u16 data_len);
u8 ZW101_Verify(void);

void ZW101_Init(void)
{
    QF_UART2_Init(ZW101_BAUD);
    UART2_Callback = ZW101_Callback;
}

u8 ZW101_Delete(u16 PageID)
{
    return ZW101_DeletChar(PageID,1);
}

u8 ZW101_Add(u8 PageID)
{
    u8 ret = 0x00;
    u8 bz = 0x00;
    u8 cnt = 0;
    
    while(1){
        switch(bz){
            case 0:
                ret = ZW101_GetEnrollImage();
                break;
            case 1:
                ret = ZW101_GenChar(1);
                break;
            case 2:
                ret = ZW101_GetEnrollImage();
                break;
            case 3:
                ret = ZW101_GenChar(2);
                break;
            case 4:
                ret = ZW101_RegModel();
                break;
            case 5:
                ret = ZW101_StoreChar(PageID);
                break;
        }
        
        if(ret!=0x00){
            bz = 0;
            cnt++;
            if(cnt>=3){
                return 0xFF;
            }
        }else if(bz == 5&& ret == 0x00){
            return 0x00;
        }else if(bz<5 && ret==0x00){
            bz++;
        }
    }
}

u8 ZW101_Get(u16* PageID)
{
    u8 ret = 0x00;
    u8 bz = 0x00;
    u8 cnt = 0;
    u16 Score = 0;
    
    while(1){
        switch(bz){
            case 0:
                ret = ZW101_GetImage();
                break;
            case 1:
                ret = ZW101_GenChar(1);
                break;
            case 2:
                ret = ZW101_Search(0,49,PageID,&Score);
                break;
        }
        
        if(ret!=0x00){
            bz = 0;
            cnt++;
            if(cnt>=3){
                *PageID = 0xFFFF;
                return 0xFF;
            }
        }else if(bz == 2 && ret == 0x00){
            return 0x00;
        }else if(bz<2 && ret==0x00){
            bz++;
        }
    }
}

/*
 * 删除指纹
 * 参数
    PageID：开始删除的ID号
    N：要删除的数量
 * 返回值
    00：删除模板成功
    01：收包有误
    10：删除模板失败
*/
u8 ZW101_DeletChar(u16 PageID,u16 N)
{
    u8 sendPacket[20] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x00,0x00,0x00};
    sendPacket[11] = PageID;
    sendPacket[13] = N;
    sendPacket[15] = ((PageID+N+0x14)>>8);
    sendPacket[16] = ((PageID+N+0x14)&0xFF);
        
    QF_UART_SendData(USART2,sendPacket,16);
    
    return ZW101_Verify();
}

/*
 * 获取有效指纹个数
 * 参数
    ValidN：有效指纹个数
 * 返回值
    00：读取成功
    01：收包有错
*/
u8 ZW101_ValidTempleteNum(u8* ValidN)
{
    u8 ret = 0;
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    ret = ZW101_Verify();
    if(ret==0x00){
        *ValidN = ZW101_RECV_BUF[10]*256+ZW101_RECV_BUF[11];
    }else{
        *ValidN = 0xFF;
    }
    return ret;
}

/*
 * 清空指纹库
 * 返回值
    00：清空成功
    01：收包错误
    11：清空失败
*/
u8 ZW101_Empty(void)
{
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    return ZW101_Verify();
}

/**
 * 搜索指纹
 * 参数
    StartPage：开始搜索地址
    PageNum：搜索数量
    PageID：搜索到的ID号
    Score：搜索到指纹的置信度
 * 返回值
    00：搜索到
    01：收包有误
    09：未搜索到
    17：残留指纹或两次采集之间手指没有移动过
*/
u8 ZW101_Search(u8 StartPage,u8 PageNum,u16* PageID,u16* Score)
{
    u8 ret = 0;
    
    u8 sendPacket[20] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    sendPacket[12] = StartPage;
    sendPacket[14] = PageNum;
    sendPacket[15] = ((StartPage+PageNum+0x0E)>>8);
    sendPacket[16] = ((StartPage+PageNum+0x0E)&0xFF);
    
    QF_UART_SendData(USART2,sendPacket,17);
    
    ret = ZW101_Verify();
    if(ret == 0x00){
        *PageID = ZW101_RECV_BUF[10]*256+ZW101_RECV_BUF[11];
        *Score = ZW101_RECV_BUF[12]*256+ZW101_RECV_BUF[13];
    }else{
        *PageID = 0xFFFF;
        *Score = 0x0000;
    }
    return ret;
}

/*
 * 验证用获取图像，存入图像缓冲区
 * 返回值
    00：获取图像成功
    01：收包有误
    02：传感器上无手指
*/
u8 ZW101_GetImage(void)
{
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    return ZW101_Verify();
}

/**
 * 保存指纹
 * 参数
    PageID：保存位置，ZW101最大保存50个指纹
 * 返回值
    00：表示储存成功；
    01：表示收包有错；
    0b：表示 PageID 超出指纹库范围；
    18：表示写 FLASH 出错；
*/
u8 ZW101_StoreChar(u8 PageID)
{
    u8 sendPacket[20] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x00};
    sendPacket[12] = PageID;
    sendPacket[13] = ((PageID+0x0E)>>8);
    sendPacket[14] = ((PageID+0x0E)&0xFF);
    
    QF_UART_SendData(USART2,sendPacket,15);
    
    return ZW101_Verify();
}

/*
 * 合并特征值
 * 返回值
    00：合并成功
    01：收包有误
    0A：合并失败
*/
u8 ZW101_RegModel(void)
{
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    return ZW101_Verify();
}

/**
 * 将图像缓冲区的图像生成特征文件存于模板缓冲区
 * 参数
    BufferID：正整数，存入模板缓冲区的位置
 * 返回值
    00：生成特征成功
    01：收包有错
    06：指纹图像太乱生成不了特征
    07：指纹图像正常特征太少生成不了特征
    08：表示当前指纹模板与之前模板之间无关联；（默认关闭此功能）
    0A：表示合并失败；
    15：表示图像缓冲区内没有有效原始图而生不成图像；
    28：表示当前指纹模板与之前模板之间有关联；（默认关闭此功能）
*/
u8 ZW101_GenChar(u8 BufferID)
{   
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x00};
    sendPacket[10] = BufferID;
    sendPacket[12] = (0x07+BufferID);
    
    QF_UART_SendData(USART2,sendPacket,13);
    
    return ZW101_Verify();
}

/*
 * 注册用获取指纹图像
 * 返回值
    00：获取图像成功
    01：收包有误
    01：传感器上无手指
*/
u8 ZW101_GetEnrollImage(void)
{
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x29,0x00,0x2D};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    return ZW101_Verify();
}

/**
 * 与模块握手
 * 返回值
    00：模块正常工作
    01：收包有错
    FF：超时未收到数据
*/
u8 ZW101_HandShake(void)
{
    u8 sendPacket[15] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x35,0x00,0x39};
    
    QF_UART_SendData(USART2,sendPacket,12);
    
    return ZW101_Verify();
}

void ZW101_Callback(u8* data, u16 data_len)
{
    memset(ZW101_RECV_BUF,0,sizeof(ZW101_RECV_BUF));
    memcpy(ZW101_RECV_BUF,data,data_len);
    ZW101_RECV_FLAG = 1;
}

u8 ZW101_Verify(void)
{
    u16 cnt = 0;
    
    while(ZW101_RECV_FLAG == 0 && cnt<30000){
        QF_DELAY_Us(100);
        cnt++;
    }
    ZW101_RECV_FLAG = 0;
    
    if(cnt>=30000){ // 超时未接收到数据
        return 0xFF;
    }else{
        return ZW101_RECV_BUF[9];
    }
}
