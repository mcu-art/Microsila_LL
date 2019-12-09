
/*
Utility functions
*/


#ifndef _UTILS_H
#define _UTILS_H

#include <mi_ll_settings.h>
#include "data_types.h"


extern uint16_t make_word(uint16_t msb, uint16_t lsb);
extern uint32_t float_to_uint32(float f);

/* Pseudo 16-bit crc */
extern BOOL validate_pseudo_crc16(const uint8_t* data, uint16_t size);
extern void fill_in_pseudo_crc16(uint8_t* data, uint16_t size);

/* True 32-bit crc 
 	 * @descr:  Standart 32-bit crc with 
							ethernet polynomial 0x4C11DB7 (reversed value is 0xEDB88320). 
							Algorythm is byte based, implementation uses precalculated table 
							allocated in flash memory; If allocate this table in SRAM, performance will be increased
							by approximately 20..40%  for STM32 device family. 
							
		* @param: uint8_t* data - the data (byte based);
							uint32_t size - data size in bytes, MUST BE DIVISIBLE BY 4;
							uint32_t previous_crc32 - previous CRC-32 value, 
																	for new CRC calculation must be zero.

		* @return: CRC-32 of the data
*/
extern uint32_t calc_crc32(const uint8_t* data, uint16_t size, uint32_t previous_crc32);
extern BOOL validate_crc32(const uint8_t* data, uint16_t size);
extern void fill_in_crc32(uint8_t* data, uint16_t size);


extern void mi_memcpy(const void* src, void* dest, uint16_t size);
extern void mi_memset(void* memBlock, uint8_t value, uint16_t size);
extern void mi_memzero(void* memBlock, uint16_t size);
extern BOOL mi_memcmp(const uint8_t* data, const uint8_t* tpl, uint16_t size);
extern BOOL compare_float(float f1, float f2);
extern BOOL compare_double(double d1, double d2);
extern void set_bitmask_bit(uint8_t* bitmask, uint8_t bit_num, BOOL isTrue);

// Write string representation of num into output buffer;
// returns number of digits written, no trailing zeroes
extern SIZETYPE uint32_to_str(uint32_t num, BYTE* output);

// Get size of zero terminated string
extern SIZETYPE strsize(const char* zero_terminated_string);

#endif
