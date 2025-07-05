#include "bl/flash.h"
#include "bl/boot.h"

#include "hal/nvmctrl.h"
#include "flash_line_api.h"
#include <stdbool.h>

static bool FLASH_PageWriteFlag;
static uint32_t FLASH_PageWriteAddress;
static uint8_t FLASH_PageWriteSize;
static uint8_t FLASH_PageWriteBuffer[64];
static uint8_t FLASH_PageWriteStatus;

static bool FLASH_BootEntryFlag;

void FLASH_Init(void) {
    FLASH_PageWriteFlag = false;
    FLASH_PageWriteAddress = 0;
    FLASH_PageWriteSize = 0;
    FLASH_PageWriteStatus = FLASH_LINE_PAGE_NOT_WRITTEN;

    FLASH_BootEntryFlag = false;
}

void FLASH_Update(void) {
    if (FLASH_PageWriteFlag) {
        if (FLASH_PageWriteSize != 64) {
            FLASH_PageWriteStatus = FLASH_LINE_PAGE_ADDRESS_ERROR;
        }
        // TODO:
        else if (FLASH_PageWriteAddress % 64 != 0 || FLASH_PageWriteAddress < 0x3000UL) {
            FLASH_PageWriteStatus = FLASH_LINE_PAGE_ADDRESS_ERROR;
        }
        else if (FLASH_PageWriteAddress % 256 == 0) {
            // TODO: finalize previous row write if not completed
            // if (IsRowValid(current_row)) {
            // }

            // start new row writing
            // TODO: incorporate page size into this
            NVMCTRL_EraseRow(FLASH_PageWriteAddress);
            // load cache
            // erase row
            // write cache
            // once full page is received write it automatically

            NVMCTRL_PageBufferClear();
            for (uint32_t addr = FLASH_PageWriteAddress; addr < FLASH_PageWriteAddress + FLASH_PageWriteSize; addr += 4) {
                *((uint32_t*)addr) = *((uint32_t*)(FLASH_PageWriteBuffer + (addr - FLASH_PageWriteAddress)));
            }
            NVMCTRL_WritePage(FLASH_PageWriteAddress);
            FLASH_PageWriteStatus = FLASH_LINE_PAGE_WRITE_SUCCESS;
        }
        else {
            // continue writing current row
            NVMCTRL_PageBufferClear();
            for (uint32_t addr = FLASH_PageWriteAddress; addr < FLASH_PageWriteAddress + FLASH_PageWriteSize; addr += 4) {
                *((uint32_t*)addr) = *((uint32_t*)(FLASH_PageWriteBuffer + (addr - FLASH_PageWriteAddress)));
            }
            NVMCTRL_WritePage(FLASH_PageWriteAddress);
            FLASH_PageWriteStatus = FLASH_LINE_PAGE_WRITE_SUCCESS;
        }

        FLASH_PageWriteFlag = false;
    }

    if (FLASH_BootEntryFlag) {
        BOOT_TryEnterApplication();

        // Defensive coding, in case boot entry was not successful
        FLASH_BootEntryFlag = false;
    }
}

/// FLASH LINE API callbacks

// TODO: implement
fl_BootSignature_t* FLASH_BL_ReadSignature(void);

void FLASH_BL_OnPageWrite(uint32_t address, uint8_t size, uint8_t* data) {
    FLASH_PageWriteFlag = true;
    FLASH_PageWriteAddress = address;
    FLASH_PageWriteSize = size;

    if (FLASH_PageWriteSize > 64) {
        FLASH_PageWriteSize = 64; // limit to maximum page size
    }

    for (uint8_t i = 0; i < FLASH_PageWriteSize; i++) {
        FLASH_PageWriteBuffer[i] = data[i];
    }

    FLASH_PageWriteStatus = FLASH_LINE_PAGE_NOT_WRITTEN;
}

uint8_t FLASH_BL_GetWriteStatus(void) {
    // TODO: reset write status after reading it

    return FLASH_PageWriteStatus;
}

void FLASH_BL_ExitBoot(void) {
    FLASH_BootEntryFlag = false;
}
