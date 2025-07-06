#if !defined(BL_API_H_)
#define BL_API_H_

/**
 * @brief Bootloader entry key special value
 */
#define BL_BOOT_ENTRY_MAGIC 0x6969000083459776LL

// Consumers can define this to disable the boot entry API,
// either used when the bootloader is not used or when the key is defined by the application.
#ifndef BL_BOOT_ENTRY_API_DISABLED

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Bootloader entry key, used to determine if the bootloader is being used.
 * This variable is defined in the shared RAM section.
 */
extern uint64_t BL_BootEntryKey;

/**
 * @brief Reset the boot entry key to indicate that the bootloader is not being used.
 * This function sets the boot entry key to 0, indicating that the bootloader is not active.
 */
void BL_ResetBootEntryKey(void);

/**
 * @brief Set the boot entry key to indicate that the bootloader is being used.
 */
void BL_SetBootEntryKey(void);

/**
 * @brief Check if the boot entry key is set to enter bootloader mode.
 * This function returns true if the boot entry key is set to the magic value,
 * indicating that the bootloader is being used.
 *
 * @return true if the boot entry key is set, false otherwise.
 */
bool BL_IsBootEntryKeySet(void);

#endif

#endif // BL_API_H_
