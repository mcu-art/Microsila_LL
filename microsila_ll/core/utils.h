
/*
Utility functions
*/


#ifndef _UTILS_H
#define _UTILS_H

#include <mi_ll_settings.h>
#include "data_types.h"
#include "byte_buf.h"


/* Macros */

// Get the array length
#define ARRLEN(_ARR) (sizeof(_ARR) / sizeof(_ARR[0]))

// Get the last element of the array
#define LAST_ELEM(_ARR) (_ARR[ARRLEN(_ARR)-1])

// Make uint16_t out of two bytes
extern inline uint16_t bytes_to_uint16(uint16_t msb, uint16_t lsb);

// Represent float data as int32_t
extern inline int32_t float_to_int32(float f);

// Represent int32_t data as float
extern inline float int32_to_float(int32_t f);

// Compare floats and doubles:
// https://bitbashing.io/comparing-floats.html

/* Tolerance means maximum absolute difference when two floats 
are still considered equal;
The smallest recommended `tolerance` is FLT_EPSILON defined in float.h.
*/
extern BOOL almost_equal_floats(float a, float b, float tolerance);


extern inline int64_t double_to_int64(double d);

extern inline double int64_to_double(int64_t i);

/* The smallest recommended `tolerance` is DBL_EPSILON defined in float.h.
*/
extern BOOL almost_equal_doubles(double a, double b, double tolerance);

/* Light-weight memory operations similar to stdlib;
   It is strongly encouraged to use stdlib instead.
   But if it is not possible for some reason, 
   you may go ahead with these functions.
*/

// Copy number of bytes from src into dest
extern void mi_memcpy(const void* src, void* dest, SIZETYPE size);

// Set number of bytes in the dest to `val`
extern void mi_memset(void* memBlock, uint8_t value, SIZETYPE size);

// Set number of bytes in the dest to 0
extern void mi_memzero(void* memBlock, SIZETYPE size);

// Compare two byte arrays
extern BOOL mi_memcmp(const uint8_t* data, const uint8_t* tpl, SIZETYPE size);

// Compare two zero-terminated strings
extern BOOL mi_strcmp(const char* first, const char* sec);
	

// Check if all bytes in `data` are equal to `val`
extern BOOL mi_memequal(const uint8_t* data, SIZETYPE size, const BYTE val);

// Get length of zero terminated string, zero not included; 
// maximum string lenght is limited to 1024 bytes
extern SIZETYPE mi_strlen(const char* zero_terminated_string);


// Bit operations
// Set bit number `bit_num` in the `bits`; return result
extern inline uint8_t setbit_uint8(uint8_t bits, uint8_t bit_num);

// Reset bit number `bit_num` in the `bits`; return result
extern inline uint8_t resetbit_uint8(uint8_t bits, uint8_t bit_num);


// Write string representation of `num` into `output` buffer if there is enough space;
// This function is about 35 times faster than sprintf()
extern void uint32_to_str(uint32_t num, ByteBuf* output);

// Same like uint32_to_str() but signed;
// This function is about 35 times faster than sprintf()
extern void int32_to_str(int32_t num, ByteBuf* output);


extern void byte_to_hex_str(BYTE num, ByteBuf* output, BOOL leading_zeroes);

// Write hex string representation of `num` into `output` buffer if there is enough space;
extern void uint32_to_hex_str(uint32_t num, ByteBuf* output, BOOL leading_zeroes);


// Convert float or double into string and write output to the buffer if there is enough space;
// `digits_after_dp' is number of digits after decimal point (8 max);
// This function is more than 10 times faster than sprintf(), but has a limitation:
// it displays floating-point values with integral part up to 0xFFFFFFFF.
// Bigger floats are represented with word `large`.

extern void float_to_str(double d, uint8_t digits_after_dp, ByteBuf* output);

// Format zero-terminated string;
// This is very light-weight analog of sprintf;
// Supports only %d, %f and %x placeholders; 
extern OP_RESULT mi_fmt_str(ByteBuf* output, const char* s, ...);
#define MI_FMT_STR_MAX_ARGS  5  // how many additional arguments may mi_fmt_str receive

// Internal usage only
typedef struct {
	BYTE type;
	SIZETYPE pos;
	BYTE len;
	BYTE param;
} MiFmtStrPhDesc;

#endif
