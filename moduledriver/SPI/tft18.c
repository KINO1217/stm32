#include "tft18.h"
#include <stdlib.h>
#include <string.h>
#include "delay.h"
#include "tft18_font.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct __MTFT18
{
   GPIO_TypeDef* SCL_Port;
   uint16_t SCL_Pin;
   GPIO_TypeDef* SDA_Port;
   uint16_t SDA_Pin;
   GPIO_TypeDef* RES_Port;
   uint16_t RES_Pin;
   GPIO_TypeDef* DC_Port;
   uint16_t DC_Pin;
   GPIO_TypeDef* CS_Port;
   uint16_t CS_Pin; 
}m_tft18;

enum writeTpye
{
    CMD=0,
    DATA
};

static void tft18WriteCmdOrData(c_tft18* this,u8 data,u8 type);
static void tft18Write16Data(c_tft18* this,u16 data);
static void tft18SetAddr(c_tft18*this,u8 xStart,u8 yStart,u8 xEnd,u8 yEnd);
static void tft18Clear(c_tft18* this,u16 color);
static void tft18DrawPoint(c_tft18* this,u8 row,u8 col,u16 color);
static void tft18DrawStr(c_tft18* this,u8 row,u8 col,u16 color,u16 bkColor,char* fmt,...);
static void tft18ShowChinese(c_tft18* this,u8 row,u8 col,u16 color,u16 bkColor,char* str);

c_tft18 tft18_create(GPIO_TypeDef* SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef* SDA_Port,uint16_t SDA_Pin,GPIO_TypeDef* RES_Port,uint16_t RES_Pin,GPIO_TypeDef* DC_Port,uint16_t DC_Pin,GPIO_TypeDef* CS_Port,uint16_t CS_Pin)
{
    // Create a new tft18 object
    c_tft18 new = {0};

    //申请空间赋值
    new.this = malloc(sizeof(m_tft18));
    memset(new.this, 0, sizeof(m_tft18));
    ((m_tft18*)(new.this))->SCL_Port = SCL_Port;
    ((m_tft18*)(new.this))->SCL_Pin = SCL_Pin;
    ((m_tft18*)(new.this))->SDA_Port = SDA_Port;
    ((m_tft18*)(new.this))->SDA_Pin = SDA_Pin;
    ((m_tft18*)(new.this))->RES_Port = RES_Port;
    ((m_tft18*)(new.this))->RES_Pin = RES_Pin;
    ((m_tft18*)(new.this))->DC_Port = DC_Port;
    ((m_tft18*)(new.this))->DC_Pin = DC_Pin;
    ((m_tft18*)(new.this))->CS_Port = CS_Port;
    ((m_tft18*)(new.this))->CS_Pin = CS_Pin;
    new.clear = tft18Clear;
    new.drawPoint = tft18DrawPoint;
    new.showStr = tft18DrawStr;
    new.showChinese = tft18ShowChinese;

    //初始化GPIO口
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SCL_Pin;
    GPIO_Init(SCL_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SDA_Pin;
    GPIO_Init(SDA_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = RES_Pin;
    GPIO_Init(RES_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = DC_Pin;
    GPIO_Init(DC_Port, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = CS_Pin;
    GPIO_Init(CS_Port, &GPIO_InitStructure);

    //复位LCD
    GPIO_SetBits(RES_Port, RES_Pin);
    vDelayMs(10);
    GPIO_ResetBits(RES_Port, RES_Pin);
    vDelayMs(10);
    GPIO_SetBits(RES_Port, RES_Pin);
    vDelayMs(150);

    //设置初始化命令
    tft18WriteCmdOrData(&new,0x11,CMD);
    tft18WriteCmdOrData(&new,0x36,CMD);
    tft18WriteCmdOrData(&new,0xC0,DATA);
    tft18WriteCmdOrData(&new,0x3A,CMD);
    tft18WriteCmdOrData(&new,0x05,DATA);
    tft18WriteCmdOrData(&new,0x29,CMD);

    return new;
}

static void tft18WriteCmdOrData(c_tft18* this,u8 data,u8 type)
{
    m_tft18* m_this = {0};
    m_this = (m_tft18*)(this->this);

    GPIO_ResetBits(m_this->CS_Port, m_this->CS_Pin);
    if(type == CMD)
        GPIO_ResetBits(m_this->DC_Port, m_this->DC_Pin);
    else if(type == DATA)
        GPIO_SetBits(m_this->DC_Port, m_this->DC_Pin);

    for(u8 i=0;i<8;i++)
    {
        if(data&0x80)
            GPIO_SetBits(m_this->SDA_Port, m_this->SDA_Pin);
        else
            GPIO_ResetBits(m_this->SDA_Port, m_this->SDA_Pin);
        GPIO_ResetBits(m_this->SCL_Port, m_this->SCL_Pin);
        GPIO_SetBits(m_this->SCL_Port, m_this->SCL_Pin);
        data<<=1;
    }

    GPIO_SetBits(m_this->CS_Port, m_this->CS_Pin);
}

static void tft18Write16Data(c_tft18* this,u16 data)
{
    tft18WriteCmdOrData(this,data>>8,DATA);
    tft18WriteCmdOrData(this,data&0xFF,DATA);
}

static void tft18SetAddr(c_tft18*this,u8 xStart,u8 yStart,u8 xEnd,u8 yEnd)
{
    //设置RAM显示范围
    tft18WriteCmdOrData(this,0x2A,CMD);
    tft18WriteCmdOrData(this,0x00,DATA);
    tft18WriteCmdOrData(this,xStart+LCDXOFFSET,DATA);
    tft18WriteCmdOrData(this,0x00,DATA);
    tft18WriteCmdOrData(this,xEnd+LCDXOFFSET,DATA);

    tft18WriteCmdOrData(this,0x2B,CMD);
    tft18WriteCmdOrData(this,0x00,DATA);
    tft18WriteCmdOrData(this,yStart+LCDYOFFSET,DATA);
    tft18WriteCmdOrData(this,0x00,DATA);
    tft18WriteCmdOrData(this,yEnd+LCDYOFFSET,DATA);

    //开始向设定区域填充颜色
    tft18WriteCmdOrData(this,0x2C,CMD);
}

static void tft18Clear(c_tft18* this,u16 color)
{
    tft18SetAddr(this,1,1,LCD_WIDTH,LCD_HIGHT);
    for(u8 col=0;col<LCD_WIDTH;col++)
    {
        for(u8 row=0;row<LCD_HIGHT;row++)
        {
            tft18Write16Data(this,color);
        }
    }
}

static void tft18DrawPoint(c_tft18* this,u8 row,u8 col,u16 color)
{
    tft18SetAddr(this,col,row,col,row);
    tft18Write16Data(this,color);
}

static void tft18DrawStr(c_tft18* this,u8 row,u8 col,u16 color,u16 bkColor,char* fmt,...)
{
    //拼接字符串
    char tempStr[64]; // 一次最多显示64个字符，四行
    memset(tempStr,0,sizeof(tempStr));
    va_list vaList;
    va_start(vaList, fmt);
    vsprintf(tempStr,fmt, vaList);
    va_end(vaList);

    int rowLen = row-1;
    int colLen = col-1;

    //计算显示位置
    for(u8 i=0;i<strlen(tempStr);i++)
    {
        u8 index = tempStr[i]-' ';

        for(u8 j=0;j<16;j++)
        {
            for(u8 k=0;k<8;k++)
            {
                if((tftFont16[index][j] >> k) & 0x01)
                    tft18DrawPoint(this,(rowLen+j+(colLen+k)/128*16)%160+1,(colLen+k)%128+1,color);
                else
                    tft18DrawPoint(this,(rowLen+j+(colLen+k)/128*16)%160+1,(colLen+k)%128+1,bkColor);
            }
        }
        colLen+=8;
    }
}

static void tft18ShowChinese(c_tft18* this,u8 row,u8 col,u16 color,u16 bkColor,char* str)
{
    u8 index = 0;
    int fontNum = sizeof(hanziFont)/sizeof(hanziStruct);

    int rowLen = row-1;
    int colLen = col-1;

    while(str[index]!='\0')
    {
        for(u8 fontSize = 0;fontSize<fontNum;fontSize++)
        {
            if(hanziFont[fontSize].hz[0] == str[index] && hanziFont[fontSize].hz[1] == str[index+1]) //查找编码
            {
                for(u8 j=0;j<32;j++)
                {
                    for(u8 k=0;k<8;k++)
                    {
                        if((hanziFont[fontSize].hzCode[j] >> k) & 0x01)
                            tft18DrawPoint(this,(rowLen+j%16+(colLen+k+j/16*8)/128*16)%160+1,(colLen+k+j/16*8)%128+1,color);
                        else
                            tft18DrawPoint(this,(rowLen+j%16+(colLen+k+j/16*8)/128*16)%160+1,(colLen+k+j/16*8)%128+1,bkColor);
                    }
                }
                colLen+=16;
                break;
            }
        }
        index+=2;
    }
} 
