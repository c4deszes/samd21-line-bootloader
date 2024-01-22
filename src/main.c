#include "bl/init.h"
#include "common/scheduler.h"

#include <stdlib.h>

int main (void) {
    /* Initialize the application */
    Initialize();

    /* Starts the scheduler */
    SCH_Enter();

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}
