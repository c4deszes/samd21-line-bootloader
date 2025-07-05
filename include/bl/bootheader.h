#if !defined(BL_BOOTHEADER_H_)
#define BL_BOOTHEADER_H_

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#define BOOTHEADER_VERSION_V1 1

typedef union {
    struct {
        uint8_t version;
        uint8_t padding[3];

        uint32_t app_start;
        uint32_t app_end;
        uint32_t app_rom_crc;

        uint32_t data_start;
        uint32_t data_end;
        uint32_t data_rom_crc;

        uint8_t sercom_id;
        uint8_t sercom_baudrate;  // * 4800 (2 -> 9600)
        bool one_wire;

        uint8_t sercom_tx_port;
        uint8_t sercom_tx_pin;
        uint8_t sercom_tx_pad;
        uint8_t sercom_tx_mux;

        uint8_t sercom_rx_port;
        uint8_t sercom_rx_pin;
        uint8_t sercom_rx_pad;
        uint8_t sercom_rx_mux;

        uint8_t sercom_txe_port;
        uint8_t sercom_txe_pin;
        
        uint8_t sercom_cs_port;
        uint8_t sercom_cs_pin;
    } fields;
    uint8_t data[252];
} bl_BootHeader_t;

static_assert(sizeof(bl_BootHeader_t) == 252, "Boot header size must be 252 bytes");

extern bl_BootHeader_t bootHeaderData;

/**
 * @brief Loads the bootheader and checks it's CRC value
 */
void BOOTHEADER_Load(void);

/**
 * @brief Checks if the bootheader is valid.
 * 
 * @return true if the bootheader is valid, false otherwise.
 */
bool BOOTHEADER_IsValid(void);

#endif // BL_BOOTHEADER_H_
