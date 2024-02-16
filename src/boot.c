#include "bl/boot.h"
#include "hal/dsu.h"
#include "hal/nvmctrl.h"
#include "hal/wdt.h"

#include "bl/config.h"
#include "bl/api.h"
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
static uint32_t calculated_approm_crc;

static bool BOOT_AppCheckCrc(void) {
    if (!BOOTHEADER_IsValid()) {
        return false;
    }

    if (bootHeaderData.fields.app_start == bootHeaderData.fields.app_end) {
        return false;
    }

    if (bootHeaderData.fields.app_start > bootHeaderData.fields.app_end) {
        return false;
    }

    // TODO: check app minimum size

    calculated_approm_crc = DSU_CalculateCRC32(0xFFFFFFFFUL,
                                               (void*)bootHeaderData.fields.app_start,
                                               bootHeaderData.fields.app_end - bootHeaderData.fields.app_start);

    return (calculated_approm_crc == bootHeaderData.fields.app_rom_crc);
}

static void __attribute__((noreturn)) BOOT_EnterApplication(void) {
    // TODO: Disable WDT before

    boot_entry_key = 0ULL;
    uint32_t app_start = bootHeaderData.fields.app_start + 4UL;
    uint32_t initial_stack_pointer = bootHeaderData.fields.app_start;
    void (*application_code_entry)(void);
    application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *)(app_start));

    // TODO: disable interrupts
    __disable_irq();
    __DMB();

    __set_MSP(*((uint32_t *)initial_stack_pointer));
    SCB->VTOR = (((uint32_t)initial_stack_pointer) & SCB_VTOR_TBLOFF_Msk);

    application_code_entry();

    while(1);
}

void _fatal() {
    while(1);
}

void BOOT_Initialize(void) {
    NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS_HALF;
    //NVMCTRL_SetReadWaitStates(NVMCTRL_CTRLB_RWS_HALF_Val);

    boot_state = boot_state_init;

    // TODO: use addresses from linkerscript
    calculated_bootrom_crc = DSU_CalculateCRC32(0xFFFFFFFFUL, 0x0000UL, 0x2000UL - 4);

    // TODO: if the CRC is wrong then halt here
    if (calculated_bootrom_crc != bootrom_crc) {
         boot_state = boot_state_rom_error;
        _fatal();
    }

    BOOTHEADER_Load();

    if (!BOOTHEADER_IsValid()) {
        boot_state = boot_state_header_error;
    }
    else if (BOOTHEADER_IsValid() && boot_entry_key != BOOT_ENTRY_MAGIC) {
        // TODO: could call tryEnterApp here
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

void BOOT_TryEnterApplication(void) {
    boot_entry_key = 0ULL;
    NVIC_SystemReset();
}

static uint32_t phantomISR = 9999;

void HardFault_Handler(void) {
    while(1) {
        phantomISR = __get_IPSR();
    }
}

void SVCall_Handler(void) {
    while(1) {
        phantomISR = __get_IPSR();
    }
}

void PendSV_Handler(void) {
    while(1) {
        phantomISR = __get_IPSR();
    }
}

void SysTick_Handler(void) {
    while(1) {
        phantomISR = __get_IPSR();
    }
}
