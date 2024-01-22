#include "bl/boot.h"
#include "hal/dsu.h"
#include "hal/nvmctrl.h"

#include "bl/config.h"
#include "bl/bootheader.h"

#include <stdint.h>
#include <stdbool.h>

#include "atsamd21e18a.h"

// TODO: for some reason these values are incorrect
extern uint32_t __bootrom_start;
extern uint32_t __bootrom_end;

uint32_t bootrom_crc __attribute__((section(".bl_rom.crc")));
uint64_t boot_entry_key __attribute__((section(".bl_shared_ram")));

static boot_state_t boot_state;
static uint32_t calculated_bootrom_crc;
static uint32_t calculted_approm_crc;

static bool BOOT_AppCheckCrc(void) {
    //calculted_approm_crc = DSU_CalculateCRC32(0xFFFFFFFFUL, 0x0000UL, (0x2000UL - 4) / 4);

    // TODO: compare

    return false;
}

void __attribute__((noreturn)) BOOT_EnterApplication(void) {
    // TODO: Disable WDT before
    // 1. clear the shared memory to 0
    // 2. setup vector table, etc.
    // 3. jump to app
    while(1);
}

void BOOT_Initialize(void) {
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_HALF;
    NVMCTRL_SetAutoPageWrite(false);
    //NVMCTRL_SetReadWaitStates(NVMCTRL_CTRLB_RWS_HALF_Val);

    boot_state = boot_state_init;

    //calculated_bootrom_crc = DSU_CalculateCRC32(0xFFFFFFFFUL, 0x0000UL, (0x2000UL - 4) / 4);

    // TODO: if the CRC is wrong then halt here
    // if (calculated_bootrom_crc != bootrom_crc) {
    //      boot_state = boot_state_rom_error;
    //     _fatal();
    // }

    // Checking header and the application
    BOOTHEADER_Load();

    if (!BOOTHEADER_IsValid()) {
        boot_state = boot_state_header_error;
    }
    else if (BOOTHEADER_IsValid() && boot_entry_key != BOOT_ENTRY_MAGIC) {
        if (BOOT_AppCheckCrc()) {
            BOOT_EnterApplication();
        }
        else {
            boot_state = boot_state_app_error;
        }
    }
    else if (BOOTHEADER_IsValid() && boot_entry_key == BOOT_ENTRY_MAGIC) {
        boot_state = boot_state_wait;
    }
}

boot_state_t BOOT_GetState(void) {
    return boot_state;
}

bool BOOT_TryEnterApplication(void) {
    // TODO: implement, check header, check app
    return false;
}