#include "bl/bootheader.h"

#include "hal/dsu.h"

// Addresses coming from the linkerscript
extern uint32_t __bootheader_start;
extern uint32_t __bootheader_end;

bl_BootHeader_t bootHeaderData __attribute__((section(".bl_header.data")));
uint32_t bootHeaderCrc __attribute__((section(".bl_header.crc")));

static uint32_t calculatedHeaderCrc;

void BOOTHEADER_Load(void) {
    // Load from start address
    // TODO: calcualte CRC
    // calculatedHeaderCrc = DSU_CalculateCRC32(0xFFFFFFFFUL,
    //                                    (void*)__bootheader_start,
    //                                    __bootheader_end - __bootheader_start);
}

bool BOOTHEADER_IsHeaderValid(void) {
    // TODO: include crc
    return bootHeaderData.fields.version == BOOTHEADER_VERSION_V1;
}
