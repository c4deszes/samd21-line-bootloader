#include "flash_line_api.h"

#include <stdbool.h>

// TODO: implement
fl_BootSignature_t* FLASH_BL_ReadSignature(void);

uint16_t current_row = 0xFFFF;
uint8_t page_write_status = FLASH_LINE_PAGE_NOT_WRITTEN;

//static uint8_t row_cache[128 * 4];
static uint8_t current_page = 0;

uint16_t GetRowNumber(uint32_t address) {
    return address / (128 * 4);
}

bool IsRowValid(uint16_t row) {
    return (row < 512);
}

uint16_t GetPageNumber(uint32_t address) {
    // TODO: implement
    // TODO: is this needed?
    return 0;
}

void FLASH_BL_OnPageWrite(uint32_t address, uint8_t size, uint8_t* data) {
    // Queue page write
    // 
    uint16_t new_row = GetRowNumber(address);

    // TODO: if page is invalid
    if (new_row == 0xFFFF) {
        // return address failure
        // page_write_status = FLASH_LINE_PAGE_ADDRESS_ERROR;
    }
    else if (new_row != current_row) {
        // TODO: finalize previous row write if not completed
        // if (IsRowValid(current_row)) {
        // }

        // start new row writing
        // load cache
        // erase row
        // write cache
        // once full page is received write it automatically
    }
    else {
        // continue writing current row
    }
}

uint8_t FLASH_BL_GetWriteStatus(void) {
    return page_write_status;
}

void FLASH_BL_ExitBoot(void) {
    // TODO: implement
}
