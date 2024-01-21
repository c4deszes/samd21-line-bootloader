#include "bl/boot.h"
#include "hal/dsu.h"

#include "bl/config.h"

#include <stdint.h>
#include <stdbool.h>

#include "atsamd21e18a.h"

uint64_t bootEntryKey __attribute__((section(".bl_shared_ram")));

extern uint32_t __bootrom_start;
extern uint32_t __bootrom_end;
uint32_t bootrom_crc __attribute__((section(".bl_rom.crc")));
static uint32_t calculatedBlCrc;

void BOOT_CheckBootloader(void) {
    calculatedBlCrc = DSU_CalculateCRC32(0xFFFFFFFFUL, 0x0000UL, (0x2000UL - 4) / 4);
}

bool BOOT_IsBootloaderValid(void) {
    //return (calculatedBlCrc == bootrom_crc);

    return true;
}

void BOOT_CheckApplication(void) {
    //DSU_CalculateCRC32()
    return;
}

bool BOOT_IsApplicationValid(void) {
    return false;
}

bool BOOT_IsEntryKeySet(void) {
    if (bootEntryKey == BOOT_ENTRY_MAGIC) {
        return true;
    }
    return false;
}

void __attribute__((noreturn)) BOOT_EnterApplication(void) {
    // TODO: Disable WDT before
    // 1. clear the shared memory to 0
    // 2. setup vector table, etc.
    // 3. jump to app
    while(1);
}
