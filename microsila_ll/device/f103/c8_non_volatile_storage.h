
/* Common device-independent definitions and API for UARTs. */


#ifndef STM32F103C8_NON_VOLATILE_STORAGE_H
#define STM32F103C8_NON_VOLATILE_STORAGE_H

#include <mi_ll_settings.h>

#define NSV_SIZE  1024

// Start erase;
// The end of operation must be polled with nvs_busy() function
extern void nvs_start_erase(void);

// Load data from NVS to RAM;
// Returns: OPR_OK if success, OPR_ERROR if data corrupted
extern OP_RESULT nvs_load(void);

// Start write sequence;
// The end of operation must be polled with nvs_busy() function
extern OP_RESULT nvs_store(void);

// Check if erase or store operation is complete
extern inline BOOL nvs_busy(void);

// Returns: pointer to internal data buffer
extern inline BYTE* nvs_get_data(void);

// Returns: TRUE if data in buffer does not match data in FLASH
extern inline BOOL nvs_modified(void);

// Call this function to mark data in the buffer as modified
extern inline void nvs_set_modified(void);

// Must be called periodically form main()
extern void nvs_do_processing(void);

#endif
