#include "hal/gclk.h"
#include "hal/pm.h"
#include "hal/wdt.h"
#include "hal/tcc.h"
#include "hal/nvic.h"
#include "hal/sysctrl.h"
#include "hal/sercom_usart.h"
#include "hal/gpio.h"

#include "common/scheduler.h"

#include <stdlib.h>

#include "atsamd21e18a.h"

#include "line_protocol.h"
#include "flash_line_api.h"
#include "flash_line_diag.h"

#include "common/ringbuffer.h"

RINGBUFFER_8(COMM_UsartBufferTx, 128);
RINGBUFFER_8(COMM_UsartBufferRx, 128);

void Initialize(void) {
    SYSCTRL_EnableInternalOSC32K();
    GCLK_Reset();
    SYSCTRL_ConfigureOSC8M();

    GCLK_ConfigureGenerator(GCLK_GEN3, GCLK_GENCTRL_SRC_OSC8M_Val, 1u);
    GCLK_ConfigureGenerator(GCLK_GEN4, GCLK_GENCTRL_SRC_OSC8M_Val, 8u);     // GCLK4 -> 1MHz

    // Setup communication
    const gpio_pin_output_configuration cs_config = {
        .drive = NORMAL,
        .input = ONDEMAND
    };
    GPIO_EnableFunction(PORT_GROUP_A, 8, PORT_PMUX_PMUXE_C_Val);
    GPIO_EnableFunction(PORT_GROUP_A, 9, PORT_PMUX_PMUXE_C_Val);
    GPIO_SetupPinOutput(PORT_GROUP_A, 10, &cs_config);
    GPIO_PinWrite(PORT_GROUP_A, 10, HIGH);

    GCLK_SelectGenerator(GCLK_CLKCTRL_ID_SERCOM0_CORE_Val, GCLK_GEN3);
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_SERCOM0_Msk;

    SERCOM_USART_SetupAsync(
        SERCOM0,
        8000000UL,
        19200,
        0,
        1,
        &COMM_UsartBufferTx,
        &COMM_UsartBufferRx
    );
    SERCOM_USART_Enable(SERCOM0);

    LINE_Transport_Init(true);
    LINE_App_Init();
    FLASH_LINE_Init(FLASH_LINE_APPLICATION_MODE);

    // Setup scheduler
    SCH_Init();

    GCLK_SelectGenerator(GCLK_CLKCTRL_ID_TCC0_TCC1_Val, GCLK_GEN4);
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_TCC0_Msk;
    TCC_Reset(TCC0);
    TCC_SetupTrigger(TCC0, 1000);   // 1000us = 1ms period
    TCC_Enable(TCC0);

    // Enable interrupts
    NVIC_Initialize();
}

void TCC0_Interrupt(void) {
    SCH_Trigger();
}
