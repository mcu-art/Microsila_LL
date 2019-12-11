
#include "utils.h"
#include <limits.h>
#include <math.h>
#include <float.h>
#include <stdarg.h>
//#include <../Src/user/leds.h>

#if MI_DEVICE == PC
#include <stdio.h> // DEBUG
#endif

inline uint16_t bytes_to_uint16(uint16_t msb, uint16_t lsb){
	uint16_t result = msb << 8;
	return result |= lsb;
}


inline int32_t float_to_int32(float f)
{
	int32_t* iptr = (int32_t*)&f;
	return (* iptr);
}


inline float int32_to_float(int32_t i) {
	float* fptr = (float*)&i;
	return (* fptr);
}

// Calculate ULPs (Units of Least Precision) between two floats
// returns absolute value of ulps
int32_t ulps_distance_float(float a, float b)
{
    // Save work if the floats are equal.
    // Also handles +0 == -0
    if (a == b) return 0;

    static const int32_t max = INT32_MAX;

    // Max distance for NaN
    if (isnan(a) || isnan(b)) return max;

		
    // If one's infinite and they're not equal, max distance.
    if (isinf(a) || isinf(b)) return max;
	
    
    int32_t ia = float_to_int32(a);
	  int32_t ib = float_to_int32(b);

    // Don't compare differently-signed floats.
    if ((ia < 0) != (ib < 0)) { return max; }

    // Return the absolute value of the distance in ULPs.
    int32_t distance = ia - ib;
    if (distance < 0) distance = -distance;
    return distance;
}


BOOL almost_equal_floats(float a, float b, float tolerance) {
	#define FLOAT_ULPS_EPSILON  4
	// Handle the near-zero case
        const float difference = (float) fabs(a - b);
	if (difference <= tolerance) return TRUE;

	return ulps_distance_float(a, b) <= FLOAT_ULPS_EPSILON;
}


inline int64_t double_to_int64(double d) {
	int64_t* iptr = (int64_t*)&d;
	return (* iptr);
}

inline double int64_to_double(int64_t i) {
	double* dptr = (double*)&i;
	return (* dptr);
}


// Calculate ULPs (Units of Least Precision) between two doubles
// returns absolute value of ulps
int64_t ulps_distance_double(double a, double b)
{
	if (a == b) return 0;
	static const int64_t max = INT64_MAX;

	// Max distance for NaN
	if (isnan(a) || isnan(b)) return max;

	// If one's infinite and they're not equal, max distance.
	if (isinf(a) || isinf(b)) return max;

	
	int64_t ia = double_to_int64(a);
	int64_t ib = double_to_int64(b);

	if ((ia < 0) != (ib < 0)) { return max; }

	// Return the absolute value of the distance in ULPs.
	int64_t distance = ia - ib;
	if (distance < 0) distance = -distance;
	return distance;
}


BOOL almost_equal_doubles(double a, double b, double tolerance) {
	#define DOUBLE_ULPS_EPSILON  4
	// Handle the near-zero case
	const double difference = fabs(a - b);
	if (difference <= tolerance) return TRUE;
	return ulps_distance_double(a, b) <= DOUBLE_ULPS_EPSILON;
}

/* Memory operations */
void mi_memcpy(const void* src, void* dest, SIZETYPE size) {
	const uint8_t* srcPtr = src;
	uint8_t* destPtr = (uint8_t*) dest;	
  for (SIZETYPE x=0; x<size; x++) *(destPtr++) = *(srcPtr++);
}

void mi_memset(void* memBlock, uint8_t value, SIZETYPE size) {
	uint8_t* ptr = memBlock;
  for (SIZETYPE x=0; x<size; x++) *(ptr++) = value;
}


void mi_memzero(void* memBlock, SIZETYPE size) {
  mi_memset(memBlock, 0, size);
}


/* 	* @descr: Compare two data arrays.
		* @param: const uint8_t* data - first array to compare
							const uint8_t* tpl - 	second array to compare
                                                        SIZETYPE size - data size in bytes

		* @return: TRUE if data match, FALSE otherwise
*/
BOOL mi_memcmp(const uint8_t* data, const uint8_t* tpl, SIZETYPE size) {
	for (SIZETYPE x = 0; x<size; ++x ){
		if ( data[x] != tpl[x]){return FALSE;}
	}
	return TRUE;
}


BOOL mi_strcmp(const char* first, const char* sec) {
	#define MI_STRCMP_MAX_SIZE  1024
	
	for (SIZETYPE x = 0; x<MI_STRCMP_MAX_SIZE; ++x ){
		if ( first[x] != sec[x]){ return FALSE; }
		if (!first[x]) return TRUE;
	}
	return FALSE; // String is too large to be compared
}


BOOL mi_memequal(const uint8_t* data, SIZETYPE size, const BYTE val) {
	const uint8_t* to = data + size;
	while (data != to) {
		if (*data != val) { return FALSE; }
		++data;
	}
	return TRUE;
}

/*
BOOL compare_float(float f1, float f2) {
	const float PRECISION = 0.00001f;
	if (((f1 - PRECISION) < f2) && ((f1 + PRECISION) > f2)) {
		return TRUE;
	} else { return FALSE; }
}

BOOL compare_double(double d1, double d2) {
	const double PRECISION = 0.00000001;
	if (((d1 - PRECISION) < d2) && ((d1 + PRECISION) > d2)) {
		return TRUE;
	} else { return FALSE; }
}
*/

uint8_t _get_bit_mask(uint8_t bit_num) {
	switch (bit_num) {
			case 1:
				return 0x01;
			case 2:
				return  0x02;

			case 3:
				return  0x04;

			case 4:
				return  0x08;

			case 5:
				return  0x10;

			case 6:
				return  0x20;

			case 7:
				return  0x40;

			case 8:
				return  0x80;

			default:
				return 0x00;
	}
}


inline uint8_t setbit_uint8(uint8_t bits, uint8_t bit_num) {
	uint8_t m = _get_bit_mask(bit_num);
	return bits |= m;
}


inline uint8_t resetbit_uint8(uint8_t bits, uint8_t bit_num) {
	uint8_t m = _get_bit_mask(bit_num);
	return bits &= (~m);
}


SIZETYPE mi_strlen(const char* zero_terminated_string) {
	const SIZETYPE MAX_STR_SIZE = 1024;
	SIZETYPE size = 0;
	while (size < MAX_STR_SIZE) {
		if (!zero_terminated_string[size]) { break; }
		++size;
	}
	return size;
}


void uint32_to_str(uint32_t num, ByteBuf* output) {
	uint32_t d;
	BOOL first_non_zero_digit_found = FALSE;
	for (uint32_t div = 1000000000; div >= 10; div /= 10) {
		d = num / div;
		if (d) { bb_write_byte_if_fits(output, (d + '0')); first_non_zero_digit_found = TRUE; }
		else { 
			if (first_non_zero_digit_found) {
				bb_write_byte_if_fits(output, (d + '0'));} 
		} 
		// write zero in the middle
		num -= d * div;
	}
	// last digit must be written anyway
	bb_write_byte_if_fits(output, (num + '0'));
}


void int32_to_str(int32_t num, ByteBuf* output) {
	if (num < 0) {
		bb_write_byte_if_fits(output, '-');
		num = (~num) + 1;
	}
	uint32_to_str(num, output);
}


void _write_nibble(BYTE nibble, ByteBuf* output) {
	BYTE sym;
	switch (nibble) {
		case 10:
			sym = 'A';
		  break;
		case 11:
			sym = 'B';
		  break;
		case 12:
			sym = 'C';
		  break;
		case 13:
			sym = 'D';
		  break;
		case 14:
			sym = 'E';
		  break;
		case 15:
			sym = 'F';
		  break;
		default:
			sym = nibble + '0';
	};
	bb_write_byte_if_fits(output, sym);
}


void byte_to_hex_str(BYTE num, ByteBuf* output, BOOL leading_zeroes) {
        BYTE nibble;
        nibble = num >> 4;
        if (!nibble) {
            if (leading_zeroes) {
                bb_write_byte_if_fits(output, '0');
            }
        } else {
          _write_nibble(nibble, output);
        }

        nibble =  num & 0x0F;
        _write_nibble(nibble, output);
}



void uint32_to_hex_str(uint32_t num, ByteBuf* output, BOOL leading_zeroes) {
    if (!num) {
        if (leading_zeroes) {
            bb_append_str(output, "00000000");
        } else {
           bb_write_byte_if_fits(output, '0');
        }
        return;
    }
    #define UINT32_SIZE sizeof(uint32_t)
    BYTE b[UINT32_SIZE];
    for (SIZETYPE i=0; i<UINT32_SIZE; ++i) {
        b[i] = (BYTE) num;
        num >>=8;
    }

    if (leading_zeroes) {
        for (int32_t i=3; i>=0; --i) {
            byte_to_hex_str(b[i], output, TRUE);
        }
    } else {
        for (int32_t i=3; i>=0; --i) {
            if (!b[i]) { continue; }
            byte_to_hex_str(b[i], output, FALSE);
        }
    }
}


void float_to_str(double d, uint8_t digits_after_dp, ByteBuf* output) {

    #define FTS_MAX_DIGITS_AFTER_DP  8
    // Check if number of digits after decimal point is within expected range
    if (!digits_after_dp) { digits_after_dp = 1; }

    if (digits_after_dp > FTS_MAX_DIGITS_AFTER_DP) {
            digits_after_dp = FTS_MAX_DIGITS_AFTER_DP;
    }

    if (isnan(d)) {
            bb_append_str(output, "NAN");
            return;
    }

    if (isinf(d)) {
            bb_append_str(output, "infinity");
            return;
    }

    if (d < 0) {
            d *= -1.0;
            bb_write_byte_if_fits(output, '-');
    }

    if (d > (double) 0xFFFFFFFF) {
            bb_append_str(output, "large");
            return;
    }

    uint32_t intpart = (uint32_t) d;
    if (!intpart) {
            bb_append_str(output, "0.0");
            return;
    }

    // prepare fractional part
	
    d -= intpart; // remove integral part from d

    uint32_t mult = 1;
    uint8_t iters = digits_after_dp;
    while (iters) {
        mult *= 10;
        --iters;
    }
    d += 1.0;
    d *= (double) mult;

    uint32_t fract_intpart = (uint32_t) d;

    if ((d - fract_intpart) >= 0.5) {  // round up
            ++fract_intpart;
            // if only one digit after dp, round up to integral part
            if (digits_after_dp == 1) { ++intpart; }
    }

    // write prepared data
    uint32_to_str(intpart, output);
    bb_write_byte_if_fits(output, '.');
    uint32_to_str(fract_intpart, output);

    if (!fract_intpart) { // fractional part is zero
        bb_write_byte_if_fits(output, '0');
        return;
    }

    // remove extra `1`
    // shift symbols after decimal point one place to the left
    for (uint8_t i=0; i<digits_after_dp; ++i) {
            output->data[output->wIndex - digits_after_dp + i - 1] =
               output->data[output->wIndex - digits_after_dp + i] ;
    }
    --output->wIndex;

    // remove trailing zeroes if any
    uint8_t trailing_zero_count = 0;
    for (uint8_t i=1; i<(digits_after_dp); ++i) {
            if (output->data[output->wIndex - i] == '0') {
                    ++trailing_zero_count;
            }
    }
    output->wIndex -= trailing_zero_count;
}


inline BOOL is_dec_digit(const char c) {
    return ((c>='0') && (c<='9')) ? TRUE: FALSE;
}


static inline BOOL is_fmt_supported(const char c) {
    switch (c) {
        case 'd':
        case 'f':
        case 'x':
        case '%':
            return TRUE;
        default:
            return FALSE;
    };
}


// If placeholder found in string, fill in the `ph` struct
// Return: TRUE if found, FALSE otherwise
BOOL mi_fmt_str_find_placeholder(const char* s, const SIZETYPE len, MiFmtStrPhDesc* ph) {
	for (SIZETYPE i = 0; i<len; ++i) {
		if (s[i] == '%') {
			if (i == (len-1)) { return FALSE; } // '%' at the end of the string
			if (is_dec_digit(s[i+1])) { // parse num of digits
				if (i == (len-2)) { return FALSE; }
				if (is_fmt_supported(s[i+2])) {
					ph->type = s[i+2];
					ph->param = s[i+1] - '0';
					ph->len = 3;
					ph->pos = i;
					return TRUE;
				}
			} else if (is_fmt_supported(s[i+1])) {
				ph->type = s[i+1];
				ph->param = 0;
		    ph->len = 2;
				ph->pos = i;
				return TRUE;
			} else {
				return FALSE;
			}
		}
	}
	return FALSE;
}


OP_RESULT mi_fmt_str(ByteBuf* output, const char* s, ...) {
	OP_RESULT result = OPR_OK;
	va_list args; 
	va_start(args, s);
	
	const SIZETYPE len = mi_strlen(s);
	SIZETYPE start_index = 0;
	int32_t iarg;
	double farg;
	
	MiFmtStrPhDesc ph;

        uint32_t dbg_count = 0;

	// Find all placeholders in the string
	while (mi_fmt_str_find_placeholder(s + start_index, len - start_index, &ph)) {
#if MI_DEVICE == PC
              dbg_count++;
              //printf("I: %d, looking for placeholders in: '%s'\n", dbg_count, s + start_index);
#endif
		// add part before the placeholder
		bb_append_array_if_fits(output, (const BYTE*) (s + start_index), ph.pos);
		
		// replace placeholder
		switch (ph.type) {
			case 'd':
                                iarg = va_arg(args, int32_t);
				int32_to_str(iarg, output);
				break;
			case 'f':
                                farg = va_arg(args, double);
				float_to_str(farg, ph.param, output);
				break;
			case 'x':
                                iarg = va_arg(args, int32_t);
                                uint32_to_hex_str(iarg, output, ph.param);
				break;
			case '%':
				bb_write_byte_if_fits(output, '%');
				break;
		}
		// advance start_index
		SIZETYPE new_index = start_index + ph.pos + ph.len;
                if (new_index > len) { break; }
		start_index = new_index;
	}
	
	// add the rest of string
	bb_append_array_if_fits(output, (const BYTE*) s, (len - start_index));
	
	va_end(args);
	
	return result;
	
}


