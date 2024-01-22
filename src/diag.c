/**
 * @file diag.c
 * @author your name (you@domain.com)
 * @brief Diagnostic functionality implementation
 * @version 0.1
 * @date 2024-01-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "line_protocol.h"

#include "bl/boot.h"
#include "hal/dsu.h"
#include "metainfo.h"

static LINE_Diag_SoftwareVersion_t sw_version = {
    .major = BL_SW_MAJOR,
    .minor = BL_SW_MINOR,
    .patch = BL_SW_PATCH
};

uint8_t LINE_Diag_GetOperationStatus(void) {
    boot_state_t state = BOOT_GetState();
    if (state == boot_state_init) {
        return LINE_DIAG_OP_STATUS_INIT;
    }
    if (state == boot_state_rom_error || state == boot_state_header_error || state == boot_state_app_error) {
        return LINE_DIAG_OP_STATUS_BOOT_ERROR;
    }
    return LINE_DIAG_OP_STATUS_BOOT;
}

uint32_t LINE_Diag_GetSerialNumber(void) {
    return DSU_GetSerialNumber32();
}

LINE_Diag_SoftwareVersion_t* LINE_Diag_GetSoftwareVersion(void) {
    return &sw_version;
}
