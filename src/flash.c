/**
 * @file flash.c
 * @author your name (you@domain.com)
 * @brief Implements flash-line-library functions
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "flash_line_api.h"
#include "bl/boot.h"
#include "hal/nvmctrl.h"

#include <stdbool.h>

// TODO: implement
fl_BootSignature_t* FLASH_BL_ReadSignature(void);

uint16_t current_row = 0xFFFF;
uint8_t page_write_status = FLASH_LINE_PAGE_NOT_WRITTEN;

uint16_t GetRowNumber(uint32_t address) {
    return address / (128 * 4);
}

bool IsRowValid(uint16_t row) {
    return (row < 512);
}

void FLASH_BL_OnPageWrite(uint32_t address, uint8_t size, uint8_t* data) {
    // Queue page write
    // 
    uint16_t new_row = GetRowNumber(address);

    // TODO: if page is invalid
    if (!IsRowValid(new_row)) {
        // return address failure
        page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    else if (size != 128) {
        page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    // TODO: 
    else if (address % 128 != 0 || address < 0x2000UL) {
        page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    else if (new_row != current_row) {
        // TODO: finalize previous row write if not completed
        // if (IsRowValid(current_row)) {
        // }

        // start new row writing
        NVMCTRL_EraseRow(new_row);
        // load cache
        // erase row
        // write cache
        // once full page is received write it automatically
        current_row = new_row;

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
    // TODO: implement
    BOOT_TryEnterApplication();
}
