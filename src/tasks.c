#include "common/swtimer.h"
#include "bl/comm.h"
#include "hal/wdt.h"
#include "hal/nvmctrl.h"

void SCH_Task1ms(void) {
    SWTIMER_Update1ms();

    COMM_Update();
}

void SCH_Task10ms_A(void) {
    // WDT_Acknowledge();
}
