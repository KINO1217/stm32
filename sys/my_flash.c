#include "my_flash.h"

#define MAXADDR 0x08010000
#define SECTORSIZE 1024

void writeData(u32 addr,u8 *data,u16 dataLen);
void readData(u32 addr,u8 *data,u16 dataLen);

const c_my_flash my_flash = {writeData,readData};

void writeData(u32 addr,u8 *data,u16 dataLen)
{
    /**校验地址*/
    if(addr <0x08000000 || addr > MAXADDR) return;

    /**计算要擦除扇区地址*/
    u32 eraseAddr = addr / SECTORSIZE * SECTORSIZE;

    /**写入数据*/
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    FLASH_ErasePage(eraseAddr);
    for(u16 i=0;i<dataLen;i++)
    {
        FLASH_ProgramHalfWord(addr+i*2,data[i]);
    }
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    FLASH_Lock();
}

void readData(u32 addr,u8 *data,u16 dataLen)
{
    if(addr <0x08000000 || addr > MAXADDR) return;

    for(u16 i=0;i<dataLen;i++)
    {
        data[i]=(u8)(*(vu16*)(addr+i*2));
    }
}
