#include "oled.h"
#include "delay.h"
#include "oled_font.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"

static const c_my_iic *oled_iic;
static void vOledWriteCmd(u8 cmd);
static void vOledWriteData(u8 data);
static void clear(void);
static void openDisplay(void);
static void closeDisplay(void);
static void vSetPos(u8 row, u8 col);
static void showChar(int row, int col, u8 ch);
static void showString(u8 row, u8 col, u8 *str, ...);
static void showOneChinese(int row,int col,u8 font[2]);
static void showChinese(u8 row, u8 col, u8 *str);   
c_oled oled_create(c_my_iic *iic)
{
    c_oled oled;
    oled_iic = iic;
    oled.clear = clear;
    oled.openDisplay = openDisplay;
    oled.closeDisplay = closeDisplay;
    oled.showString = showString;
    oled.showChinese = showChinese;
    vDelayMs(100);

    // OLED初始化
    vOledWriteCmd(0xA8);
    vOledWriteCmd(0x3F);
    vOledWriteCmd(0xDA);
    vOledWriteCmd(0x12);
    vOledWriteCmd(0xD3);
    vOledWriteCmd(0x00);
    vOledWriteCmd(0x40);
    vOledWriteCmd(0xA1);
    vOledWriteCmd(0x81);
    vOledWriteCmd(0xFF);
    vOledWriteCmd(0xA4);
    vOledWriteCmd(0xA6);
    vOledWriteCmd(0x8D);
    vOledWriteCmd(0x14);
    vOledWriteCmd(0x20);
    vOledWriteCmd(0x02);
    vOledWriteCmd(0xC8);
    vOledWriteCmd(0xB0);
    vOledWriteCmd(0x00);
    vOledWriteCmd(0x10);
    vOledWriteCmd(0xD9);
    vOledWriteCmd(0x22);
    vOledWriteCmd(0xDB);
    vOledWriteCmd(0x20);
    vOledWriteCmd(0xAF);

    return oled;
}

static void vOledWriteCmd(u8 cmd)
{
    oled_iic->start(oled_iic);
    oled_iic->sendByteData(oled_iic, 0x78);
    oled_iic->checkAck(oled_iic);
    oled_iic->sendByteData(oled_iic, 0x00);
    oled_iic->checkAck(oled_iic);
    oled_iic->sendByteData(oled_iic, cmd);
    oled_iic->checkAck(oled_iic);
    oled_iic->stop(oled_iic);
}

static void vOledWriteData(u8 data)
{
    oled_iic->start(oled_iic);
    oled_iic->sendByteData(oled_iic, 0x78);
    oled_iic->checkAck(oled_iic);
    oled_iic->sendByteData(oled_iic, 0x40);
    oled_iic->checkAck(oled_iic);
    oled_iic->sendByteData(oled_iic, data);
    oled_iic->checkAck(oled_iic);
    oled_iic->stop(oled_iic);
}

static void clear(void)
{
    for (u8 i = 0; i < 8; i++)
    {
        vOledWriteCmd(0xB0 | i);
        vOledWriteCmd(0x00);
        vOledWriteCmd(0x10);
        for (u8 j = 0; j < 128; j++)
        {
            vOledWriteData(0x00);
        }
    }
}

static void openDisplay(void)
{
    vOledWriteCmd(0x8D);
    vOledWriteCmd(0x14);
    vOledWriteCmd(0xAF);
}

static void closeDisplay(void)
{
    vOledWriteCmd(0x8D);
    vOledWriteCmd(0x10);
    vOledWriteCmd(0xAE);
}

/**
 * 设置显示位置，行:0-7,列:0-127
 */
static void vSetPos(u8 row, u8 col)
{
    if (row > 7 || col > 127)
        return;

    vOledWriteCmd(0xB0 | row);
    vOledWriteCmd(0x00 | (col & 0x0F));
    vOledWriteCmd(0x10 | ((col >> 4) & 0x0F));
}

/**
 * 显示一个字符函数
 */
static void showChar(int row, int col, u8 ch)
{
    // 自动换行操作
    if (col > 127)
    {
        row += ((col / 127) * 2);
        col %= 128;
    }
    if (row > 7)
    {
        row = row % 8;
    }

    int index = ch - ' '; // 计算字符在数组中的索引

    for (u8 i = 0; i < 2; i++)
    {
        vSetPos(row + i, col);
        for (u8 j = 0; j < 8; j++)
        {
            vOledWriteData(ch8x16[index][i * 8 + j]);
        }
    }
}

static void showString(u8 row, u8 col, u8 *str, ...)
{
    // 1.调用库函数拼接字符串
    u8 tempStr[64];
    memset(tempStr, 0, sizeof(tempStr));
    va_list vaList;
    va_start(vaList, str);
    vsprintf((char *)tempStr, (char *)str, vaList);
    va_end(vaList);

    // 2.显示字符串
    for (u8 i = 0; i < strlen((char *)tempStr); i++)
    {
        showChar(row, col + i * 8, tempStr[i]);
    }
}

/**
 * 显示一个字符函数
*/
static void showOneChinese(int row,int col,u8 font[2])
{
    // 自动换行
    if (col > 127)
    {
        row += ((col / 127) * 2);
        col %= 128;
    }
    if (row > 7)
    {
        row = row % 8;
    }

    int zkCount = sizeof(chinaFont)/sizeof(ChineseStruct);  // 计算字库中汉字的数量

    for(int index=0;index<zkCount;index++)
    {
        if(chinaFont[index].hz[0] == font[0] && chinaFont[index].hz[1] == font[1])
        {
            for (u8 i = 0; i < 2; i++)
            {
                vSetPos(row + i, col);
                for (u8 j = 0; j < 16; j++)
                {
                    vOledWriteData(chinaFont[index].hzHex[i * 16 + j]);
                }
            }
            break;
        }
    }
}

static void showChinese(u8 row, u8 col, u8 *str)
{
    int index = 0;
    u8 tempChina[2];

    while(str[index]!='\0')
    {
        tempChina[0] = str[index];
        tempChina[1] = str[index+1];
        showOneChinese(row,index/2*16+col,tempChina);
        index += 2;
    }
}
