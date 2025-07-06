#include "hal/sercom_usart.h"

#include "hal/dsu.h"
#include "common/scheduler.h"
#include "line_protocol.h"
#include "bl/api.h"
#include "flash_line_api.h"
#include "common/swtimer.h"

#include "sam.h"

uint64_t boot_entry_key __attribute__((section(".bl_shared_ram")));
static bool bootResetFlag = false;

fl_BootEntryResponse_t FLASH_BL_EnterBoot(void) {
    BL_SetBootEntryKey();
    bootResetFlag = true;

    fl_BootEntryResponse_t response;
    response.entry_status = FLASH_LINE_BOOT_ENTRY_SUCCESS;
    response.serial_number = DSU_GetSerialNumber32();

    return response;
}

void COMM_Update(void) {
    uint8_t length = SERCOM_USART_Available(SERCOM0);
    while (length > 0) {
        uint8_t data = SERCOM_USART_Read(SERCOM0);
        // TODO: use channel number from header
        LINE_Transport_Receive(0, data);
        length--;
    }

    // TODO: use channel number from header
    LINE_Transport_Update(0, 1);
}

void LINE_Transport_WriteResponse(uint8_t channel, uint8_t size, uint8_t* payload, uint8_t checksum) {
    const uint8_t fix = 69;
    SERCOM_USART_WriteData(SERCOM0, &size, sizeof(uint8_t));
    // TODO: fix for skipped 3rd byte
    SERCOM_USART_WriteData(SERCOM0, payload, 1);
    SERCOM_USART_WriteData(SERCOM0, &fix, 1);
    SERCOM_USART_WriteData(SERCOM0, payload+1, size-1);
    SERCOM_USART_WriteData(SERCOM0, &checksum, sizeof(uint8_t));
    SERCOM_USART_FlushOutput(SERCOM0);
}

/// LINE Application Layer

void LINE_App_OnRequest(uint8_t channel, uint16_t request, uint8_t size, uint8_t* payload) {
    LINE_Diag_OnRequest(channel, request, size, payload);
}

bool LINE_App_RespondsTo(uint8_t channel, uint16_t request) {
    return LINE_Diag_RespondsTo(channel, request);
}

bool LINE_App_PrepareResponse(uint8_t channel, uint16_t request, uint8_t* size, uint8_t* payload) {
    return LINE_Diag_PrepareResponse(channel, request, size, payload);
}

void SERCOM0_Interrupt(void) {
    SERCOM_USART_InterruptHandler(SERCOM0);
}

/// Scheduler Task

void SCH_Task1ms(void) {
    SWTIMER_Update1ms();

    COMM_Update();

    if (bootResetFlag) {
        NVIC_SystemReset();
    }
}
