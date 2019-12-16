
#include "utils_test.h"
#include <microsila_ll/core/test_helpers.h>
#include <microsila_ll/core/byte_buf.h>
#include <microsila_ll/core/dbg_console.h>

#include <float.h>
#include <stdarg.h>


/* MACRO TESTS BEGIN */
		
OP_RESULT macro_test(void) {
	OP_RESULT result = OPR_OK;
	BYTE byte_arr[] = {1, 2, 3};
	CHECK (ARRLEN(byte_arr) == 3);
	CHECK (LAST_ELEM(byte_arr) == 3);
	
	float float_arr[] = {1.0f, 2.0f, 3.0f};
	CHECK (sizeof(float_arr) == 12);
	CHECK (ARRLEN(float_arr) == 3);
	CHECK (LAST_ELEM(float_arr) == 3.0f);

	return result;
}

/* MACRO TESTS END */

/* FLOATING POINT ARITHMETIC TESTS BEGIN */

/* float */
OP_RESULT float_to_int32_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (float_to_int32(1.0f) == 0x3f800000);
	CHECK (float_to_int32(-100.25f) == 0xc2c88000);
	return result;
}


OP_RESULT int32_to_float_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (int32_to_float(0x3f800000) == 1.0f);
	CHECK (int32_to_float(0xc2c88000) == -100.25f);
	CHECK (int32_to_float(0x0U) == 0.0f);
	CHECK (int32_to_float(0x80000000U) == -0.0f);
	return result;
}

extern int32_t ulps_distance_float(float a, float b);

OP_RESULT ulps_distance_float_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (ulps_distance_float(100.0f, 100.0f) == 0);
	CHECK (ulps_distance_float(1.0f, (1.0f + FLT_EPSILON)) == 1);
	CHECK (ulps_distance_float(1.0f, (1.0f - FLT_EPSILON)) == 2);
	float a_inf = int32_to_float(0x7f800000);
	float b_minus_inf = int32_to_float(0xff800000);
	CHECK (ulps_distance_float(a_inf, b_minus_inf) == INT32_MAX);
	
	return result;
}

OP_RESULT almost_equal_floats_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (almost_equal_floats(100.0f, 100.0f, FLT_EPSILON));
	CHECK (almost_equal_floats(1.0f, (1.0f - FLT_EPSILON), FLT_EPSILON));
	CHECK (almost_equal_floats(0.0f, -0.0f, FLT_EPSILON));
	CHECK (!almost_equal_floats(0.0f, -0.0f + (4 * FLT_EPSILON), FLT_EPSILON));
	CHECK (almost_equal_floats(1.0f, 1.0004f, 0.001f));
	CHECK (!almost_equal_floats(1.0f, 1.0004f, 0.0001f));
	CHECK (almost_equal_floats(1.0f, 1.0001f, 0.00015f));
	// Edge of float resolution
	CHECK (almost_equal_floats(1000000.0f, 1000000.1f, FLT_EPSILON));
	CHECK (!almost_equal_floats(1000000.0f, 1000000.8f, FLT_EPSILON));
	
	float orig = 1.0f;
	for (int x=0; x<100; ++x) {
		orig += 0.1f;
	}
	CHECK (orig != 11.0f);
	CHECK (almost_equal_floats(orig, 11.0f, 0.00001f));
	
	return result;
}


/* double */
OP_RESULT double_to_int64_test(void) {
	OP_RESULT result = OPR_OK;
    CHECK (double_to_int64(1.0) == 0x3ff0000000000000LL);
    CHECK (double_to_int64(-100.25) == 0xc059100000000000LL);
	return result;
}


OP_RESULT int64_to_double_test(void) {
	OP_RESULT result = OPR_OK;
    CHECK (int64_to_double(0x3ff0000000000000LL) == 1.0);
    CHECK (int64_to_double(0xc059100000000000LL) == -100.25);
	CHECK (int64_to_double(0x0ULL) == 0.0);
    CHECK (int64_to_double(0x8000000000000000LL) == -0.0);
	return result;
}

extern int64_t ulps_distance_double(double a, double b);

OP_RESULT ulps_distance_double_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (ulps_distance_double(100.0, 100.0) == 0);
	CHECK (ulps_distance_double(1.0, (1.0 + DBL_EPSILON)) == 1);
	CHECK (ulps_distance_double(1.0, (1.0 - DBL_EPSILON)) == 2);
        double a_inf = int64_to_double(0x7f80000000000000ULL);
        double b_minus_inf = int64_to_double(0xff80000000000000LL);
	CHECK (ulps_distance_double(a_inf, b_minus_inf) == 0x7FFFFFFFFFFFFFFFULL);
	return result;
}


OP_RESULT almost_equal_doubles_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (almost_equal_doubles(100.0, 100.0, DBL_EPSILON));
	CHECK (almost_equal_doubles(1.0, (1.0 - DBL_EPSILON), DBL_EPSILON));
	CHECK (almost_equal_doubles(0.0, -0.0, DBL_EPSILON));
	CHECK (!almost_equal_doubles(0.0, -0.0 + (4 * DBL_EPSILON), DBL_EPSILON));
	CHECK (almost_equal_doubles(1.0, 1.0004, 0.001));
	CHECK (!almost_equal_doubles(1.0, 1.0004, 0.0001));
	CHECK (almost_equal_doubles(1.0, 1.0001, 0.00015));
	// Edge of double resolution
	CHECK (almost_equal_doubles(1000000000000.0, 1000000000000.000001, DBL_EPSILON));
	CHECK (!almost_equal_doubles(1000000000000.0, 1000000000000.001, DBL_EPSILON));
	return result;
}

/* FLOATING POINT ARITHMETIC TESTS END */

/* STRING UTILS TESTS BEGIN */


OP_RESULT mi_strlen_test(void) {
	OP_RESULT result = OPR_OK;
	CHECK (mi_strlen("") == 0);
	CHECK (mi_strlen("1") == 1);
	return result;
}


OP_RESULT uint32_to_str_test(void) {
	OP_RESULT result = OPR_OK;
	BB_CREATE(buf, 64);
	uint32_t i = 0;
	uint32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 1);
	CHECK (buf->data[0] == '0');
	bb_reset(buf);
	
	i = 4000000000;
	uint32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 10);
	CHECK (buf->data[0] == '4');
	CHECK (mi_memequal(buf->data + 1, 9, '0'));
	bb_reset(buf);
	
	++i;
	uint32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 10);
	CHECK (buf->data[0] == '4');
	CHECK (mi_memequal(buf->data + 1, 8, '0'));
	CHECK (buf->data[9] == '1');
	bb_reset(buf);
	
	i = 1234567890;
	uint32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 10);
	for (SIZETYPE x=0; x<9; ++x) {
		CHECK (buf->data[x] == '1' + x);
	}
  CHECK (buf->data[9] == '0');
	bb_reset(buf);
	return result;
}


OP_RESULT int32_to_str_test(void) {
	OP_RESULT result = OPR_OK;
	BB_CREATE(buf, 64);
	int32_t i = 0;
	int32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 1);
	CHECK (buf->data[0] == '0');
	bb_reset(buf);
	
	i = 2000000000;
	int32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 10);
	CHECK (buf->data[0] == '2');
	CHECK (mi_memequal(buf->data + 1, 9, '0'));
	bb_reset(buf);
	
	i = -2;
	int32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 2);
	CHECK (buf->data[0] == '-');
	CHECK (buf->data[1] == '2');
	bb_reset(buf);
	
	++i;
	int32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 2);
	CHECK (buf->data[0] == '-');
	CHECK (buf->data[1] == '1');
	bb_reset(buf);
	
	i = -1234567890;
	int32_to_str(i, buf);
	CHECK (bb_unread_size(buf) == 11);
	CHECK (buf->data[0] == '-');
	for (SIZETYPE x=1; x<10; ++x) {
		CHECK (buf->data[x] == '1' + x - 1);
	}
  CHECK (buf->data[10] == '0');
	bb_reset(buf);
	return result;
}



OP_RESULT float_to_str_test(void) {
	OP_RESULT result = OPR_OK;
	BB_CREATE(buf, 64);
	
	float f = 0.0f;
	float_to_str(f, 4, buf);
	CHECK (bb_unread_size(buf) == 3);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "0.0", 3));
	bb_reset(buf);
	
	f = -0.0f;
	float_to_str(f, 4, buf);
	CHECK (bb_unread_size(buf) == 3);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "0.0", 3));
	bb_reset(buf);

    f = -0.1f;
    float_to_str(f, 4, buf);
    CHECK (bb_unread_size(buf) == 4);
    CHECK (mi_memcmp(buf->data, (const BYTE*) "-0.1", 4));
    bb_reset(buf);
	
	f = 6.22e23f;
	float_to_str(f, 4, buf);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "large", 5));
	bb_reset(buf);
	
	double d;
	d = 10.622578;
	float_to_str(d, 5, buf);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "10.62258", 8));
	bb_reset(buf);

    d = .2577;
    float_to_str(d, 3, buf);
    CHECK (mi_memcmp(buf->data, (const BYTE*) "0.258", 5));
    bb_reset(buf);
	
	float_to_str(-1000000000.99, 5, buf);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "-1000000000.99", 13));
	bb_reset(buf);
	
	float_to_str(-1000000000.99, 1, buf);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "-1000000001.0", 12));
	bb_reset(buf);
	
	float_to_str(100000000.12345678, 8, buf);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "100000000.12345678", 17));
	bb_reset(buf);
	
	return result;
}


OP_RESULT byte_to_hex_str_test(void) {
	OP_RESULT result = OPR_OK;
	BYTE b = 0;
	BB_CREATE(buf, 64);
	
	bb_reset(buf);
        byte_to_hex_str(b, buf, TRUE);
	CHECK (bb_unread_size(buf) == 2);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "00", sizeof("00") - 1));

        bb_reset(buf);
        byte_to_hex_str(b, buf, FALSE);
        CHECK (bb_unread_size(buf) == 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "0", 1));

        b = 0x0cf;
        bb_reset(buf);
        byte_to_hex_str(b, buf, TRUE);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "C", 1));
	
	b = 0xaf;
	bb_reset(buf);
        byte_to_hex_str(b, buf, TRUE);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "AF", 2));
	
	b = 0x1C;
	bb_reset(buf);
        byte_to_hex_str(b, buf, TRUE);
	CHECK (mi_memcmp(buf->data, (const BYTE*) "1C", 2));
	return result;
}


OP_RESULT uint32_to_hex_str_test(void) {
        OP_RESULT result = OPR_OK;
        uint32_t val = 0;
        BB_CREATE(buf, 64);

        bb_reset(buf);
        uint32_to_hex_str(val, buf, TRUE);
        CHECK (bb_unread_size(buf) == 8);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "00000000", 8));

        bb_reset(buf);
        uint32_to_hex_str(val, buf, FALSE);
        CHECK (bb_unread_size(buf) == 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "0", 1));

        val = 0x01;
        bb_reset(buf);
        uint32_to_hex_str(val, buf, FALSE);
        CHECK (bb_unread_size(buf) == 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "1", 1));

        val = 0x12F;
        bb_reset(buf);
        uint32_to_hex_str(val, buf, FALSE);
        CHECK (bb_unread_size(buf) == 3);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "12F", 1));

        val = 0x12F;
        bb_reset(buf);
        uint32_to_hex_str(val, buf, TRUE);
        CHECK (bb_unread_size(buf) == 8);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "0000012F", 8));

        val = 0x12345678;
        bb_reset(buf);
        uint32_to_hex_str(val, buf, TRUE);
        CHECK (bb_unread_size(buf) == 8);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "12345678", 8));

        val = 0x9abcdef;
        bb_reset(buf);
        uint32_to_hex_str(val, buf, FALSE);
        CHECK (bb_unread_size(buf) == 7);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "9ABCDEF", 7));

        return result;
}


// test if call to variadic function causes stack overflow or underrun
static BOOL var_arg_func(ByteBuf* buf, const char* s, ...) {
	BOOL result = FALSE;
  const SIZETYPE len = mi_strlen(s);
	if (!len) { return FALSE; }
	
	va_list args; 
	va_start(args, s);
	volatile uint32_t ival;
	volatile double dval;
	
	if (mi_strcmp(s, "int")) {
		ival = va_arg(args, int32_t);
		result = TRUE;
	} else if (mi_strcmp(s, "double")) {
		dval = va_arg(args, double);
		result = TRUE;
	} 
	bb_append_array_if_fits(buf, (const BYTE*) s, len);
	va_end(args);
	return result;
}


OP_RESULT var_arg_func_test(void) {
	OP_RESULT result = OPR_OK;
	BB_CREATE(buf, 64);
	CHECK (!var_arg_func(buf, ""));
	bb_reset(buf);
	
	CHECK (var_arg_func(buf, "int", 3));
	CHECK (mi_memcmp(buf->data, (const BYTE*) "int", mi_strlen("int")));
	bb_reset(buf);
	
	CHECK (var_arg_func(buf, "double", 100.37));
	CHECK (mi_memcmp(buf->data, (const BYTE*) "double", mi_strlen("double")));
	bb_reset(buf);
	
	CHECK (!var_arg_func(buf, "this is a test", 1000, 98725.0));
	CHECK (mi_memcmp(buf->data, (const BYTE*) "this is a test", mi_strlen("this is a test")));
	bb_reset(buf);
	
	return result;
}


OP_RESULT mi_fmt_str_find_placeholder_test(void) {
    extern BOOL mi_fmt_str_find_placeholder(const char*, const SIZETYPE, MiFmtStrPhDesc*);

    OP_RESULT result = OPR_OK;
    MiFmtStrPhDesc ph;

    CHECK (FALSE == mi_fmt_str_find_placeholder("", 0, &ph));
    CHECK (FALSE == mi_fmt_str_find_placeholder("test", 4, &ph));
    CHECK (TRUE == mi_fmt_str_find_placeholder("%dtest", sizeof("%dtest"), &ph));
    CHECK ( (ph.pos == 0) && (ph.len == 2) && (ph.type == 'd') && (ph.param == 0));

    CHECK (TRUE == mi_fmt_str_find_placeholder("test%3f", sizeof("test%3f"), &ph));
    CHECK ((ph.pos == 4) && (ph.len == 3) && (ph.type == 'f') && (ph.param == 3));

    CHECK (FALSE == mi_fmt_str_find_placeholder("test%3", sizeof("test%3"), &ph));
    CHECK (FALSE == mi_fmt_str_find_placeholder("test%", sizeof("test%"), &ph));
  CHECK (TRUE == mi_fmt_str_find_placeholder("test%%", sizeof("test%%"), &ph));

    return result;
}


OP_RESULT mi_fmt_str_test(void) {
	extern BOOL mi_fmt_str_find_placeholder(const char*, const SIZETYPE, MiFmtStrPhDesc*);

	OP_RESULT result = OPR_OK;
    BB_CREATE(buf, 80);

    bb_reset(buf);
    CHECK (OPR_OK == mi_fmt_str(buf, "* ByteBuf(size = %d):", 3));
    CHECK (bb_unread_size(buf) == sizeof("* ByteBuf(size = 3):") - 1);
    CHECK (mi_memcmp(buf->data, (const BYTE*) "* ByteBuf(size = 3):",
                     sizeof("* ByteBuf(size = 3):") - 1));


	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, ""));
	CHECK (buf->rIndex == 0);

	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, "test"));
  CHECK (bb_unread_size(buf) == sizeof("test") - 1);
  CHECK (mi_memcmp(buf->data, (const BYTE*) "test", sizeof("test") - 1));

	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, "test %d", 1));
  CHECK (bb_unread_size(buf) == sizeof("test 1") - 1);
  CHECK (mi_memcmp(buf->data, (const BYTE*) "test 1", sizeof("test 1") - 1));
	
	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, "%7d test %d", 11223344, 1));
	CHECK (bb_unread_size(buf) == sizeof("11223344 test 1") - 1); 
	CHECK (mi_memcmp(buf->data, (const BYTE*) "11223344 test 1", sizeof("11223344 test 1") - 1));
	
	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, "#%d: test %2f", 1, 22.578));
	CHECK (bb_unread_size(buf) == sizeof("#1: test 22.58") - 1); 
	CHECK (mi_memcmp(buf->data, (const BYTE*) "#1: test 22.58", sizeof("#1: test 22.58") - 1));
	
	bb_reset(buf);
	CHECK (OPR_OK == mi_fmt_str(buf, "#%d-%8d: test %5f", 1, 2, 10034.57890));
	CHECK (bb_unread_size(buf) == sizeof("#1-2: test 10034.5789") - 1); 
	CHECK (mi_memcmp(buf->data, (const BYTE*) "#1-2: test 10034.5789", sizeof("#1-2: test 10034.5789") - 1));

        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "test %x", 15));
        CHECK (bb_unread_size(buf) == sizeof("test F") - 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "test F", sizeof("test F") - 1));

        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "test %1x", 15));
        CHECK (bb_unread_size(buf) == sizeof("test 0000000F") - 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "test 0000000F", sizeof("test 0000000F") - 1));

        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "test %8x", 0x0a2b3c4d));
        CHECK (bb_unread_size(buf) == sizeof("test 0A2B3C4D") - 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "test 0A2B3C4D", sizeof("test 0A2B3C4D") - 1));

        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "test %x", 0x0a2b3c4d));
        CHECK (bb_unread_size(buf) == sizeof("test A2B3C4D") - 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "test A2B3C4D", sizeof("test A2B3C4D") - 1));
	
        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "test %s", "ok"));
        CHECK (bb_unread_size(buf) == sizeof("test ok") - 1);
        CHECK (mi_memcmp(buf->data, (const BYTE*) "test ok", sizeof("test ok") - 1));

        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "#%d. test %s received, %3f apples eaten",
                                    1, "the message", 0.535));
        CHECK (mi_memcmp(buf->data,
                         (const BYTE*) "#1. test the message received, 0.535 apples eaten",
                          sizeof("#1. test the message received, 0.535 apples eaten") - 1));
        bb_reset(buf);
        CHECK (OPR_OK == mi_fmt_str(buf, "%d %s, %d %s, %d %s, fraction: %5f, hex code: %1x",
                                    1, "apple", 2, "plums",
                                    3, "water melons",
                                    0.535, 0x445577));
        CHECK (mi_memcmp(buf->data,
                         (const BYTE*) "1 apple, 2 plums, 3 water melons, fraction: 0.535, hex code: 00445577",
                          sizeof("1 apple, 2 plums, 3 water melons, fraction: 0.535, hex code: 00445577") - 1));


	return result;
}


/* STRING UTILS TESTS END */


/* BIT OPERATIONS TESTS BEGIN */

OP_RESULT set_reset_bit_uint8_test(void) {
	OP_RESULT result = OPR_OK;
	uint8_t val = 0;
	val = setbit_uint8(val, 2);
	CHECK (val == 0x02);
	val = setbit_uint8(val, 8);
	CHECK (val == 0x82);
	val = resetbit_uint8(val, 2);
	CHECK (val == 0x80);
	val = resetbit_uint8(val, 8);
	CHECK (val == 0x00);
	
	return result;
}


/* BIT OPERATIONS TESTS END */

OP_RESULT utils_test_all(void) {
	
	
  const uint32_t ITERS = 1000;
	OP_RESULT r; 
	
    DO_TEST(mi_fmt_str_test, ITERS);


	DO_TEST(macro_test, ITERS);
	 
	DO_TEST(mi_strlen_test, ITERS);
	DO_TEST(float_to_int32_test, ITERS);
	DO_TEST(int32_to_float_test, ITERS);
	DO_TEST(ulps_distance_float_test, ITERS);
	DO_TEST(almost_equal_floats_test, ITERS);
	DO_TEST(double_to_int64_test, ITERS);
	DO_TEST(int64_to_double_test, ITERS);
	DO_TEST(ulps_distance_double_test, ITERS);
	DO_TEST(almost_equal_doubles_test, ITERS);
	DO_TEST(uint32_to_str_test, ITERS);
	DO_TEST(int32_to_str_test, ITERS);
	DO_TEST(set_reset_bit_uint8_test, ITERS);
	DO_TEST(float_to_str_test, ITERS); 
	DO_TEST(mi_fmt_str_find_placeholder_test, ITERS);
	DO_TEST(byte_to_hex_str_test, ITERS);
	
	DO_TEST(var_arg_func_test, ITERS);

        DO_TEST(uint32_to_hex_str_test, ITERS);

    /**/

	return OPR_OK;
}
