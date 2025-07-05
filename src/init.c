#include "bl/init.h"
#include "bl/comm.h"

#include "bl/boot.h"
#include "bl/bootheader.h"
#include "bl/flash.h"

#include "hal/gclk.h"
#include "hal/pm.h"
#include "hal/wdt.h"
#include "hal/tcc.h"
#include "hal/nvic.h"
#include "hal/nvmctrl.h"
#include "hal/sysctrl.h"

#include "common/scheduler.h"
#include "metainfo.h"

// TODO: remove this once direct register access is removed
#include "sam.h"

// TODO: prevent linker from removing this symbol
static const char GIT_COMMIT_SHA[] __attribute__((used)) = BL_GIT_COMMIT;

void Initialize(void) {
    // TODO: enable watchdog
    // Uses GCLK2 (OSCULP32K) on POR, then whatever the Application configures
    //WDT_InitializeNormal(&wdt_config);
    BOOT_Initialize();

    NVMCTRL_SetAutoPageWrite(false);

    SYSCTRL_EnableInternalOSC32K();
    GCLK_Reset();
    SYSCTRL_ConfigureOSC8M();

    GCLK_ConfigureGenerator(GCLK_GEN3, GCLK_GENCTRL_SRC_OSC8M_Val, 1u);
    GCLK_ConfigureGenerator(GCLK_GEN4, GCLK_GENCTRL_SRC_OSC8M_Val, 8u);     // GCLK4 -> 1MHz

    // Setup communication
    COMM_Initialize();

    FLASH_Init();

    // Setup scheduler
    SCH_Init();

    GCLK_SelectGenerator(GCLK_CLKCTRL_ID_TCC0_TCC1_Val, GCLK_GEN4);
    // TODO: move this to HAL library
    PM_REGS->PM_APBCMASK |= PM_APBCMASK_TCC0_Msk;
    TCC_Reset(TCC0);
    TCC_SetupTrigger(TCC0, 1000);   // 1000us = 1ms period
    TCC_Enable(TCC0);

    NVIC_Initialize();
}

void TCC0_Interrupt(void) {
    SCH_Trigger();
}
