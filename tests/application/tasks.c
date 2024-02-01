#include "hal/sercom_usart.h"

#include "line_protocol.h"
#include "common/swtimer.h"

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

LINE_Diag_SoftwareVersion_t* LINE_Diag_GetSoftwareVersion(void) {
    return &sw_version;
}

void COMM_Update(void) {
    uint8_t length = SERCOM_USART_Available(SERCOM0);
    while (length > 0) {
        uint8_t data = SERCOM_USART_Read(SERCOM0);
        LINE_Transport_Receive(data);
        length--;
    }
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
}
