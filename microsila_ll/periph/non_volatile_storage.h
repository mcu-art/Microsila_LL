
/* Common device-independent definitions and API for UARTs. */


#ifndef NON_VOLATILE_STORAGE_H
#define NON_VOLATILE_STORAGE_H

#include <mi_ll_settings.h>

// Default storage size is equal to embedded flash sector size - 4 bytes for CRC-32 check
#define NVS_SIZE  EMBEDDED_FLASH_SECTOR_SIZE - 4

// Returns: size of non-volatile flash storage 
// (size that is available for user, 4 service bytes of crc32 at the end not included)
extern inline SIZETYPE nvs_size(void);

// Start erase;
// The end of operation must be polled with nvs_busy() function
extern void nvs_erase(void);

// Returns: TRUE if non-volatile flash storage is erased, FALSE otherwise
extern BOOL nvs_erased(void);

// Load data from NVS to RAM;
// Returns: OPR_OK if success, OPR_ERROR if data corrupted
extern OP_RESULT nvs_load(void);

// Start write sequence;
// The end of operation must be polled with nvs_busy() function
extern OP_RESULT nvs_store(void);

// Check if data in flash is same as in nms_buf;
// Returns: OPR_OK if data correct, OPR_ERROR if data does not match, OPR_BUSY if flash is busy.
extern OP_RESULT nvs_verify(void);

// Check if erase or store operation is complete
extern inline BOOL nvs_busy(void);

// Returns: pointer to internal data buffer
extern inline BYTE* nvs_get_data(void);


#endif
