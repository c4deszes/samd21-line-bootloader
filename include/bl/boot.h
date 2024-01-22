#if !defined(BL_BOOT_H_)
#define BL_BOOT_H_

#include <stdbool.h>

typedef enum {
    boot_state_init,
    boot_state_rom_error,
    boot_state_header_error,
    boot_state_app_error,
    boot_state_wait
} boot_state_t;

/**
 * @brief Initializes the bootloader
 * 
 * - Decision is made whether the bootloader itself is correct
 * - Boot header and application is checked for validity
 * - In case of valid header and application this function immediately jumps to the app.
 */
void BOOT_Initialize(void);

/**
 * @brief Returns the bootloader's state
 * 
 * @return boot_state_t Bootloader state
 */
boot_state_t BOOT_GetState(void);

/**
 * @brief Attempts application entry
 * 
 * @return true Function never returns with this value
 * @return false If entry failed
 */
bool BOOT_TryEnterApplication(void);

#endif // BL_BOOT_H_
