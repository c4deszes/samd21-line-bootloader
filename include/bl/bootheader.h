#if !defined(BL_BOOTHEADER_H_)
#define BL_BOOTHEADER_H_

#include <stdbool.h>
#include <stdint.h>

#define BOOTHEADER_VERSION_V1 1

typedef union {
    struct {
        uint8_t version;
        uint8_t padding[3];
        uint32_t app_start;
        uint32_t app_end;
        uint32_t data_start;
        uint32_t data_end;

        uint8_t sercom_id;
        uint8_t sercom_baudrate;  // * 4800 (2 -> 9600)
        bool one_wire;

        uint8_t sercom_tx_port;
        uint8_t sercom_tx_pin;
        uint8_t sercom_tx_pad;

        uint8_t sercom_rx_port;
        uint8_t sercom_rx_pin;
        uint8_t sercom_rx_pad;

        uint8_t sercom_txe_port;
        uint8_t sercom_txe_pin;
        
        uint8_t sercom_cs_port;
        uint8_t sercom_cs_pin;
    } fields;
    uint8_t data[124];
} bl_BootHeader_t;

extern bl_BootHeader_t bootHeaderData;

void BOOTHEADER_Load(void);

/**
 * @brief Checks whether the header is valid by it's CRC value and version number
 * 
 * @return true 
 * @return false 
 */
bool BOOTHEADER_IsHeaderValid(void);

#endif // BL_BOOTHEADER_H_
