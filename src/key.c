#include "bl/api.h"

#ifndef BL_BOOT_ENTRY_API_DISABLED

uint64_t BL_BootEntryKey __attribute__((section(".bl_shared_ram")));

void BL_ResetBootEntryKey(void)
{
    BL_BootEntryKey = 0;
}

void BL_SetBootEntryKey(void)
{
    BL_BootEntryKey = BL_BOOT_ENTRY_MAGIC;
}

bool BL_IsBootEntryKeySet(void)
{
    return BL_BootEntryKey == BL_BOOT_ENTRY_MAGIC;
}

#endif
