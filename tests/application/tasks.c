#include "hal/sercom_usart.h"

#include "hal/dsu.h"
#include "common/scheduler.h"
#include "line_protocol.h"
#include "bl/api.h"
#include "flash_line_api.h"
#include "common/swtimer.h"

#include "sam.h"

static LINE_Diag_SoftwareVersion_t sw_version = {
    .major = 1,
    .minor = 2,
    .patch = 3
};

uint8_t LINE_Diag_GetOperationStatus(void) {
    return LINE_DIAG_OP_STATUS_OK;
}

uint32_t LINE_Diag_GetSerialNumber(void) {
    return DSU_GetSerialNumber32();
}

void LINE_Transport_WriteRequest(uint16_t request) {
    // do nothing
}

LINE_Diag_SoftwareVersion_t* LINE_Diag_GetSoftwareVersion(void) {
    return &sw_version;
}

uint64_t boot_entry_key __attribute__((section(".bl_shared_ram")));
static bool bootResetFlag = false;

uint8_t FLASH_BL_EnterBoot(void) {
    boot_entry_key = BOOT_ENTRY_MAGIC;
    bootResetFlag = true;

    return FLASH_LINE_BOOT_ENTRY_SUCCESS;
}

void COMM_Update(void) {
    uint8_t length = SERCOM_USART_Available(SERCOM0);
    while (length > 0) {
        uint8_t data = SERCOM_USART_Read(SERCOM0);
        LINE_Transport_Receive(data);
        length--;
    }

    LINE_Transport_Update(1);
}

void LINE_Transport_WriteResponse(uint8_t size, uint8_t* payload, uint8_t checksum) {
    const uint8_t fix = 69;
    SERCOM_USART_WriteData(SERCOM0, &size, sizeof(uint8_t));
    // TODO: fix for skipped 3rd byte
    SERCOM_USART_WriteData(SERCOM0, payload, 1);
    SERCOM_USART_WriteData(SERCOM0, &fix, 1);
    SERCOM_USART_WriteData(SERCOM0, payload+1, size-1);
    SERCOM_USART_WriteData(SERCOM0, &checksum, sizeof(uint8_t));
    SERCOM_USART_FlushOutput(SERCOM0);
}

void SCH_Task1ms(void) {
    SWTIMER_Update1ms();

    COMM_Update();

    if (bootResetFlag) {
        NVIC_SystemReset();
    }
}
