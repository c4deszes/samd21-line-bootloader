#include "flash_line_api.h"
#include "bl/boot.h"
#include "hal/nvmctrl.h"

#include <stdbool.h>

// TODO: implement
fl_BootSignature_t* FLASH_BL_ReadSignature(void);

uint8_t page_write_status = FLASH_LINE_PAGE_NOT_WRITTEN;

void FLASH_BL_OnPageWrite(uint32_t address, uint8_t size, uint8_t* data) {
    // Queue page write

    // TODO: if page is invalid
    if (size != 64) {
        page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    // TODO: 
    else if (address % 64 != 0 || address < 0x2000UL) {
        page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    else if (address % 256 == 0) {
        // TODO: finalize previous row write if not completed
        // if (IsRowValid(current_row)) {
        // }

        // start new row writing
        // TODO: incorporate page size into this
        NVMCTRL_EraseRow(address);
        // load cache
        // erase row
        // write cache
        // once full page is received write it automatically

        NVMCTRL_PageBufferClear();
        for (uint32_t addr = address; addr < address + size; addr += 4) {
            *((uint32_t*)addr) = *((uint32_t*)(data + (addr - address)));
        }
        NVMCTRL_WritePage(address);
        page_write_status = FLASH_LINE_PAGE_WRITE_SUCCESS;
    }
    else {
        // continue writing current row
        NVMCTRL_PageBufferClear();
        for (uint32_t addr = address; addr < address + size; addr += 4) {
            *((uint32_t*)addr) = *((uint32_t*)(data + (addr - address)));
        }
        NVMCTRL_WritePage(address);
        page_write_status = FLASH_LINE_PAGE_WRITE_SUCCESS;
    }
}

uint8_t FLASH_BL_GetWriteStatus(void) {
    return page_write_status;
}

void FLASH_BL_ExitBoot(void) {
    BOOT_TryEnterApplication();
}
