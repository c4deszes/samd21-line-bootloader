
#include "bl/comm.h"
#include "bl/bootheader.h"
#include "bl/config.h"

#include "line_protocol.h"
#include "flash_line_diag.h"

#include "hal/sercom_usart.h"
#include "hal/gpio.h"
#include "hal/gclk.h"
#include "common/ringbuffer.h"

#include "sam.h"

RINGBUFFER_8(COMM_UsartBufferTx, 128);
RINGBUFFER_8(COMM_UsartBufferRx, 128);

static uint8_t comm_sercom;
static uint32_t comm_baudrate;
static bool comm_onewire;
static uint8_t comm_tx_port;
static uint8_t comm_tx_pin;
static uint8_t comm_tx_pad;
static uint8_t comm_tx_mux;
static uint8_t comm_rx_port;
static uint8_t comm_rx_pin;
static uint8_t comm_rx_pad;
static uint8_t comm_rx_mux;
static uint8_t comm_cs_port;
static uint8_t comm_cs_pin;

static void COMM_LoadDefaults(void) {
    comm_sercom = COMM_DEFAULT_SERCOM;
    comm_baudrate = COMM_DEFAULT_BAUDRATE;
    comm_onewire = COMM_DEFAULT_ONE_WIRE;
    comm_tx_port = COMM_DEFAULT_TX_PORT;
    comm_tx_pin = COMM_DEFAULT_TX_PIN;
    comm_tx_pad = COMM_DEFAULT_TX_PAD;
    comm_tx_mux = COMM_DEFAULT_TX_MUX;
    comm_rx_port = COMM_DEFAULT_RX_PORT;
    comm_rx_pin = COMM_DEFAULT_RX_PIN;
    comm_rx_pad = COMM_DEFAULT_RX_PAD;
    comm_rx_mux = COMM_DEFAULT_RX_MUX;
    comm_cs_port = COMM_DEFAULT_CS_PORT;
    comm_cs_pin = COMM_DEFAULT_CS_PIN;
}

static void COMM_LoadConfig(void) {
    if (BOOTHEADER_IsValid()) {
        if (bootHeaderData.fields.sercom_id <= SERCOM3 &&
            bootHeaderData.fields.sercom_tx_port == PORT_GROUP_A &&
            bootHeaderData.fields.sercom_rx_port == PORT_GROUP_A &&
            bootHeaderData.fields.sercom_tx_pin <= 31 &&
            bootHeaderData.fields.sercom_rx_pin <= 31 &&
            (bootHeaderData.fields.sercom_tx_mux == PORT_PMUX_PMUXE_C_Val || bootHeaderData.fields.sercom_tx_mux == PORT_PMUX_PMUXE_D_Val) &&
            (bootHeaderData.fields.sercom_rx_mux == PORT_PMUX_PMUXE_C_Val || bootHeaderData.fields.sercom_rx_mux == PORT_PMUX_PMUXE_D_Val) &&
            bootHeaderData.fields.sercom_tx_pad <= SERCOM_USART_TX_PAD2 &&
            bootHeaderData.fields.sercom_rx_pad <= SERCOM_USART_RX_PAD3 &&
            bootHeaderData.fields.sercom_baudrate >= 1 && bootHeaderData.fields.sercom_baudrate <= 24) {
        
            comm_sercom = bootHeaderData.fields.sercom_id;
            comm_baudrate = bootHeaderData.fields.sercom_baudrate * 4800u;
            comm_onewire = (bool) bootHeaderData.fields.one_wire;
            comm_tx_port = bootHeaderData.fields.sercom_tx_port;
            comm_tx_pin = bootHeaderData.fields.sercom_tx_pin;
            comm_tx_pad = bootHeaderData.fields.sercom_tx_pad;
            comm_tx_mux = bootHeaderData.fields.sercom_tx_mux;
            comm_rx_port = bootHeaderData.fields.sercom_rx_port;
            comm_rx_pin = bootHeaderData.fields.sercom_rx_pin;
            comm_rx_pad = bootHeaderData.fields.sercom_rx_pad;
            comm_rx_mux = bootHeaderData.fields.sercom_rx_mux;

            if (bootHeaderData.fields.sercom_cs_port == PORT_GROUP_A &&
                bootHeaderData.fields.sercom_cs_pin <= 31) {
                comm_cs_port = bootHeaderData.fields.sercom_cs_port;
                comm_cs_pin = bootHeaderData.fields.sercom_cs_pin;
            }
        }
        else {
            COMM_LoadDefaults();
        }
    }
    else {
        COMM_LoadDefaults();
    }
}

static uint8_t sercom_generator(uint8_t sercom) {
    switch(sercom) {
        case SERCOM0: return GCLK_CLKCTRL_ID_SERCOM0_CORE_Val;
        case SERCOM1: return GCLK_CLKCTRL_ID_SERCOM1_CORE_Val;
        case SERCOM2: return GCLK_CLKCTRL_ID_SERCOM2_CORE_Val;
        case SERCOM3: return GCLK_CLKCTRL_ID_SERCOM3_CORE_Val;
    }
    return 0xFF;
}

static uint32_t sercom_pm_mask(uint8_t sercom) {
    switch(sercom) {
        case SERCOM0: return PM_APBCMASK_SERCOM0_Msk;
        case SERCOM1: return PM_APBCMASK_SERCOM1_Msk;
        case SERCOM2: return PM_APBCMASK_SERCOM2_Msk;
        case SERCOM3: return PM_APBCMASK_SERCOM3_Msk;
    }
    return 0;
}

void COMM_Initialize(void) {
    COMM_LoadConfig();

    GPIO_EnableFunction(comm_tx_port, comm_tx_pin, comm_tx_mux);
    GPIO_EnableFunction(comm_rx_port, comm_rx_pin, comm_rx_mux);

    // TODO: txe pin support
    if (comm_cs_port == PORT_GROUP_A &&
        comm_cs_pin <= 31) {
        
        const gpio_pin_output_configuration cs_config = {
            .drive = NORMAL,
            .input = ONDEMAND
        };
        GPIO_SetupPinOutput(comm_cs_port, comm_cs_pin, &cs_config);
        GPIO_PinWrite(comm_cs_port, comm_cs_pin, HIGH);
    }

    GCLK_SelectGenerator(sercom_generator(comm_sercom), GCLK_GEN3);
    PM_REGS->PM_APBCMASK |= sercom_pm_mask(comm_sercom);

    SERCOM_USART_SetupAsync(
        comm_sercom,
        8000000UL,
        comm_baudrate,
        comm_tx_pad,
        comm_rx_pad,
        &COMM_UsartBufferTx,
        &COMM_UsartBufferRx
    );
    SERCOM_USART_Enable(comm_sercom);

    LINE_Transport_Init(comm_onewire);
    LINE_App_Init();
    FLASH_LINE_Init(FLASH_LINE_BOOTLOADER_MODE);
}


void SERCOM0_Interrupt(void) {
    SERCOM_USART_InterruptHandler(SERCOM0);
}

void SERCOM1_Interrupt(void) {
    SERCOM_USART_InterruptHandler(SERCOM1);
}

void SERCOM2_Interrupt(void) {
    SERCOM_USART_InterruptHandler(SERCOM2);
}

void SERCOM3_Interrupt(void) {
    SERCOM_USART_InterruptHandler(SERCOM3);
}

void COMM_Update(void) {
    uint8_t length = SERCOM_USART_Available(comm_sercom);
    while (length > 0) {
        uint8_t data = SERCOM_USART_Read(comm_sercom);
        LINE_Transport_Receive(data);
        length--;
    }

    LINE_Transport_Update(1);
}

void LINE_Transport_WriteResponse(uint8_t size, uint8_t* payload, uint8_t checksum) {
    const uint8_t fix = 69;
    SERCOM_USART_WriteData(comm_sercom, &size, sizeof(uint8_t));
    // TODO: fix for skipped 3rd byte
    SERCOM_USART_WriteData(comm_sercom, payload, 1);
    SERCOM_USART_WriteData(comm_sercom, &fix, 1);
    SERCOM_USART_WriteData(comm_sercom, payload+1, size-1);
    SERCOM_USART_WriteData(comm_sercom, &checksum, sizeof(uint8_t));
    SERCOM_USART_FlushOutput(comm_sercom);
}

void LINE_Transport_WriteRequest(uint16_t request) {
    // do nothing
}
