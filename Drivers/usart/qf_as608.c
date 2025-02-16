#include "qf_as608.h"
#include "qf_delay.h"
#include "qf_uart.h"

char as608_recv_buf[64];
u8 as608_recv_flag = 0;

static u8 verify(u16* ret, u16 timeout);

void QF_AS608_Callback(u8* data, u16 data_len)
{
    memset(as608_recv_buf, 0, sizeof(as608_recv_buf));
    memcpy(as608_recv_buf, data, data_len);
    as608_recv_flag = 1;
}

static u8 verify(u16* ret, u16 timeout)
{
    u16 cnt = 0;
    while (as608_recv_flag == 0 && cnt < timeout) { // 获取数据
        cnt++;
        QF_DELAY_Ms(1);
    }
    as608_recv_flag = 0;
    if (cnt >= timeout) {
        return 0xFF;
    }

    if (as608_recv_buf[8] > 0x03) {
        *ret = ((as608_recv_buf[10] << 8) + as608_recv_buf[11]);
    } else {
        *ret = 0xFFFF;
    }

    return as608_recv_buf[9]; // 返回确认码
}

void QF_AS608_Init(void)
{
    QF_UART2_Init(AS608_BAUD);
    UART2_Callback = QF_AS608_Callback;
}

void QF_AS608_Add_FR(u16 FR_ID)
{
    u8 cnt = 0;
    u8 ret = 0;

    while (1) {
        if (cnt == 0x00) {
            ret = QF_AS608_GetImage(); // 获取图像
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }
        if (cnt == 0x01) {
            ret = QF_AS608_GenChar(0x01); // 生成特征值
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }
        if (cnt == 0x02) {
            ret = QF_AS608_GetImage(); // 获取图像
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }
        if (cnt == 0x03) {
            ret = QF_AS608_GenChar(0x02); // 生成特征值
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }
        if (cnt == 0x04) {
            ret = QF_AS608_Match(); // 对比特征值
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }

        if (cnt == 0x05) {
            ret = QF_AS608_RegModel(); // 生成模板
            if (ret == 0x00) {
                cnt++;
            } else {
                cnt = 0;
            }
        }

        if (cnt == 0x06) {
            ret = QF_AS608_StoreChar(0x01, FR_ID); // 存储模板
            if (ret == 0x00) { // 存储完成跳出循环
                break;
            } else {
                cnt = 0;
            }
        }
    }
}

void QF_AS608_Del_FR(u16 FR_ID)
{
    QF_AS608_DeleteChar(FR_ID, 1);
}

void QF_AS608_Get_FR(u16* FR_ID)
{
    u8 cnt = 0;
    u8 ret = 0;
    u16 RET_ID = 0;

    if (cnt == 0) {
        ret = QF_AS608_GetImage();
        if (ret == 0x00) {
            cnt++;
        } else {
            *FR_ID = 0xFFFF;
            return;
        }
    }

    if (cnt == 1) {
        ret = QF_AS608_GenChar(0x01);
        if (ret == 0x00) {
            cnt++;
        } else {
            *FR_ID = 0xFFFF;
            return;
        }
    }

    if (cnt == 2) {
        ret = QF_AS608_Search(0x01, 0, 255, &RET_ID);
        if (ret == 0x00) {
            *FR_ID = RET_ID;
        } else {
            *FR_ID = 0xFFFF;
        }
    }
}

/**
 * 获取图像
 * @return 确认码
    00：录入成功
    01：收包错误
    02：没有检测到手指
    03：录入不成功
*/
u8 QF_AS608_GetImage(void)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x03;
    sendTemp[9] = 0x01;
    sendTemp[10] = 0x00;
    sendTemp[11] = 0x05;
    QF_UART_SendData(USART2, sendTemp, 12);

    return verify(&ret, 500);
}

/**
 * 生成特征值（将ImageBuffer图像生成特征值存放CharBuffer1或CharBuffer2）
 * @param CharBufferID
    0x01：CharBuffer1
    0x02：CharBuffer2
 * @return 确认码
    00：生成特征成功
    01：收包有误
    06：指纹太乱
    07：特征点太少
    15：没有图像生成
 */
u8 QF_AS608_GenChar(u8 CharBufferID)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x04;
    sendTemp[9] = 0x02;
    sendTemp[10] = CharBufferID;
    sendTemp[11] = 0x00;
    sendTemp[12] = 0x07 + CharBufferID;
    QF_UART_SendData(USART2, sendTemp, 13);

    return verify(&ret, 500);
}

/**
 * 比对CharBuffer1和CharBuffer2的特征值
 * @return 确认码
    00：指纹匹配成功
    01：收包有误
    08：指纹不匹配
 */
u8 QF_AS608_Match(void)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x03;
    sendTemp[9] = 0x03;
    sendTemp[10] = 0x00;
    sendTemp[11] = 0x07;
    QF_UART_SendData(USART2, sendTemp, 12);

    return verify(&ret, 500);
}

/**
 * 生成模板
 * @return 确认码
    00：合并成功
    01：收包有误
    0A：合并失败（不是同一个手指）
 */
u8 QF_AS608_RegModel(void)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x03;
    sendTemp[9] = 0x05;
    sendTemp[10] = 0x00;
    sendTemp[11] = 0x09;
    QF_UART_SendData(USART2, sendTemp, 12);

    return verify(&ret, 500);
}

/**
 * 存储特征值
 * @param CharBufferID
    0x01：CharBuffer1
    0x02：CharBuffer2
 * @param pageID
    0-255
 * @return 确认码
    00：存储模板成功
    01：收包有误
    0B：PageID超出指纹库范围
    18：写FLASH出错
 */
u8 QF_AS608_StoreChar(u8 CharBufferID, u16 pageID)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x06;
    sendTemp[9] = 0x06;
    sendTemp[10] = CharBufferID;
    sendTemp[11] = 0x00;
    sendTemp[12] = pageID;
    sendTemp[13] = (0x0D + CharBufferID + pageID) / 256;
    sendTemp[14] = (0x0D + CharBufferID + pageID) % 256;
    QF_UART_SendData(USART2, sendTemp, 15);

    return verify(&ret, 500);
}

/**
 * 从pageID开始删除N个模板数据
 * @param pageID
    0-255
 * @param N
    0-255 删除模板数量
 * @return 确认码
    00：删除模板成功
    01：收包有误
    10：删除模板失败
*/
u8 QF_AS608_DeleteChar(u16 pageID, u16 N)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x07;
    sendTemp[9] = 0x0C;
    sendTemp[10] = 0x00;
    sendTemp[11] = pageID;
    sendTemp[12] = 0x00;
    sendTemp[13] = N;
    sendTemp[14] = (0x14 + pageID + N) / 256;
    sendTemp[15] = (0x14 + pageID + N) % 256;
    QF_UART_SendData(USART2, sendTemp, 16);

    return verify(&ret, 500);
}

/**
 * 清空指纹库
 * @return 确认码
    00：清空所有成功
    01：收包有误
    10：清空所有失败
*/
u8 QF_AS608_Empty(void)
{
    u8 sendTemp[32];
    u16 ret = 0;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x03;
    sendTemp[9] = 0x0D;
    sendTemp[10] = 0x00;
    sendTemp[11] = 0x11;
    QF_UART_SendData(USART2, sendTemp, 12);

    return verify(&ret, 500);
}

/**
 * 获取指纹库有效模板数量
 * @param ValidN 有效指纹数量
 * @return 确认码
    00：读取成功
    01：收包有误
*/
u8 QF_AS608_ValidTempleteNum(u16* ValidN)
{
    u8 sendTemp[32];
    u16 ret = 0;
    u8 rt = 0x00;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x03;
    sendTemp[9] = 0x1D;
    sendTemp[10] = 0x00;
    sendTemp[11] = 0x21;
    QF_UART_SendData(USART2, sendTemp, 12);

    rt = verify(&ret, 500);
    *ValidN = ret;

    return rt;
}

u8 QF_AS608_HighSpeedSearch(u8 CharBufferID, u16 StartPage, u16 PageNum, u16* p)
{
    u8 sendTemp[32];
    u16 ret = 0;
    u8 rt = 0x00;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x08;
    sendTemp[9] = 0x1B;
    sendTemp[10] = CharBufferID;
    sendTemp[11] = 0x00;
    sendTemp[12] = StartPage;
    sendTemp[13] = 0x00;
    sendTemp[14] = PageNum;
    sendTemp[15] = (0x24 + CharBufferID + StartPage + PageNum) / 256;
    sendTemp[16] = (0x24 + CharBufferID + StartPage + PageNum) % 256;
    QF_UART_SendData(USART2, sendTemp, 17);

    rt = verify(&ret, 500);
    *p = ret;

    return rt;
}

u8 QF_AS608_Search(u8 CharBufferID, u16 StartPage, u16 PageNum, u16* p)
{
    u8 sendTemp[32];
    u16 ret = 0;
    u8 rt = 0x00;
    sendTemp[0] = 0xEF;
    sendTemp[1] = 0x01;
    sendTemp[2] = 0xFF;
    sendTemp[3] = 0xFF;
    sendTemp[4] = 0xFF;
    sendTemp[5] = 0xFF;
    sendTemp[6] = 0x01;
    sendTemp[7] = 0x00;
    sendTemp[8] = 0x08;
    sendTemp[9] = 0x04;
    sendTemp[10] = CharBufferID;
    sendTemp[11] = 0x00;
    sendTemp[12] = StartPage;
    sendTemp[13] = 0x00;
    sendTemp[14] = PageNum;
    sendTemp[15] = (0x0D + CharBufferID + StartPage + PageNum) / 256;
    sendTemp[16] = (0x0D + CharBufferID + StartPage + PageNum) % 256;
    QF_UART_SendData(USART2, sendTemp, 17);

    rt = verify(&ret, 500);
    *p = ret;

    return rt;
}
