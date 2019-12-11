
/* Crc calculation */


#ifndef _CRC_H
#define _CRC_H

#include <mi_ll_settings.h>
#include "data_types.h"



/* 16-bit xor */
// Xor-16 is a 16-bit exclusive OR of data used by some external modules 
// to check data integrity;
// Even though it is less reliable than crc-32, the computation cost is much
// lower especially for 8-bit devices.

// Calculate 16-bit xor
extern uint16_t xor16(const BYTE* data, SIZETYPE size);

// Check if 16-bit xor of data is equal to the provided xor16;
// size: must be divisible by 2
// Returns: TRUE if equal
extern inline BOOL xor16_equal(const BYTE* data, SIZETYPE size, const uint16_t* xor16_val);


/* Standart 32-bit crc 
 	 * @descr:  Standart 32-bit crc with 
							ethernet polynomial 0x4C11DB7 (reversed value is 0xEDB88320). 
							Algorythm is byte-based, implementation uses precalculated table 
							allocated in flash memory; If allocated in SRAM, the performance might be increased
							by approximately 20..40%  for STM32 devices. 
							
		* @param: uint8_t* data - the data (byte based);
							uint32_t size - data size in bytes, MUST BE DIVISIBLE BY 4;
							uint32_t previous_crc32 - previous CRC-32 value, 
																	for new CRC calculation must be zero.

		* @return: CRC-32 of the data
*/
extern uint32_t crc32(const uint8_t* data, SIZETYPE size, uint32_t previous_crc32);

// Check if crc-32 of the data is equal to crc32_val
extern inline BOOL crc32_equal(const uint8_t* data, SIZETYPE size, const uint32_t* crc32_val);




#endif
