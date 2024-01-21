#if !defined(BL_BOOT_H_)
#define BL_BOOT_H_

#include <stdbool.h>

void BOOT_CheckBootloader(void);

bool BOOT_IsBootloaderValid(void);

void BOOT_CheckApplication(void);

bool BOOT_IsApplicationValid(void);

bool BOOT_IsEntryKeySet(void);

void BOOT_EnterApplication(void);

#endif // BL_BOOT_H_
