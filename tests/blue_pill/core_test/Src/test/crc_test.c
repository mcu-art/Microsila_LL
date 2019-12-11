
#include "crc_test.h"
#include "microsila_ll/core/byte_buf.h"
#include "microsila_ll/core/test_helpers.h"

	
		
OP_RESULT xor16_equal_test(void) {
	OP_RESULT result = OPR_OK;
	static const BYTE data[] = {0x01, 0x02, 0x03, 0x04, 0x02, 0x06 };
	
	CHECK (xor16_equal(data, 4, (uint16_t*) (data + 4)));
	uint16_t xor16 = 0x602;
  CHECK (xor16_equal(data, 4, &xor16));
	
	return result;
}


OP_RESULT crc32_test(void) {
	OP_RESULT result = OPR_OK;
	static const BYTE data[] = {0x01, 0x02, 0x03, 0x04, 0xcd, 0xfb, 0x3c, 0xb6 };
	
	uint32_t crc = 0xB63CFBCD;
  CHECK (crc32_equal(data, 4, &crc));
	CHECK (crc32_equal(data, 4, (const uint32_t*)(data+4)));
	
	return result;
}


OP_RESULT crc_test_all(void) {
	
	const uint32_t ITERS = 100;
	OP_RESULT r;
	DO_TEST(xor16_equal_test, ITERS);
	DO_TEST(crc32_test, ITERS);

	return OPR_OK;
}
