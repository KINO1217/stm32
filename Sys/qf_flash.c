#include "qf_flash.h"

void QF_FLASH_ErasePage(u32 addr)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    FLASH_ErasePage((addr / 1024 * 1024));
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    FLASH_Lock();
}

void QF_FLASH_Write(u32 addr, u8* data, u16 data_len) // ???1K????
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    if (data_len % 2 == 0) {
        for (u16 i = 0; i < data_len; i += 2) {
            FLASH_ProgramHalfWord(addr + i, ((data[i + 1] << 8) | data[i]));
        }
    } else if (data_len % 2 == 1) {
        for (u16 i = 0; i < data_len - 1; i += 2) {
            FLASH_ProgramHalfWord(addr + i, ((data[i + 1] << 8) | data[i]));
        }

        FLASH_ProgramHalfWord(addr + data_len - 1, 0xFF00 | data[data_len - 1]);
    }
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
    FLASH_Lock();
}

void QF_FLASH_Read(u32 addr, u8* data, u16 data_len)
{
    for (u16 i = 0; i < data_len; i++)
        data[i] = (uint8_t)(*(vu8*)(addr + i));
}
