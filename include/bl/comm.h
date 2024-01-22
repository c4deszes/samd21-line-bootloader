#if !defined(BL_COMM_H_)
#define BL_COMM_H_

/**
 * @brief Initializes the communication interfaces
 * 
 * @note Must be called after the boot header has been loaded and checked
 */
void COMM_Initialize(void);

/**
 * @brief Processes incoming requests, sends responses
 * 
 * @note Must be called every 1ms or less
 */
void COMM_Update(void);

#endif // BL_COMM_H_
