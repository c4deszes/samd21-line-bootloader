#if !defined(BL_FLASH_H)
#define BL_FLASH_H

/**
 * @brief Initialize the Flashing services
 */
void FLASH_Init(void);

/**
 * @brief Update the Flashing services
 * 
 * This function should be called periodically to handle any pending flash operations.
 */
void FLASH_Update(void);

#endif // BL_FLASH_H
