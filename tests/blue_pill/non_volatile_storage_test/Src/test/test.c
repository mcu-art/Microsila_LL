
#include "test.h"
#include <microsila_ll/core/byte_buf.h>
#include <microsila_ll/core/utils.h>
#include <microsila_ll/core/test_helpers.h>
#include <microsila_ll/core/dbg_console.h>
#include <microsila_ll/periph/non_volatile_storage.h>



OP_RESULT nvs_read_test(void) {
	BB_CREATE(buf, 64);
  OP_RESULT result = nvs_load();
  CHECK ( (result == OPR_OK) || (result == OPR_CRC_ERROR));
	BYTE* data = nvs_get_data();
	bb_append_array_if_fits(buf, data, 8);
	dc_printbuf(buf);

  return result;
}


OP_RESULT nvs_verify_test(void) {
	BB_CREATE(buf, 64);
	OP_RESULT result = OPR_OK;
  nvs_load();
  CHECK (OPR_OK == nvs_verify());
	BYTE* data = nvs_get_data();
	data[33] = 89;
	CHECK (OPR_ERROR == nvs_verify());

  return result;
}

OP_RESULT nvs_erase_test(void) {
	BB_CREATE(buf, 64);
  OP_RESULT result = OPR_OK;
  nvs_erase();
	CHECK (nvs_erased());
	BYTE* data = nvs_get_data();
	CHECK (OPR_CRC_ERROR == nvs_load());
	CHECK (mi_memequal(data, nvs_size() + 4, 0xff));

  return result;
}


OP_RESULT nvs_store_test(void) {
	BB_CREATE(buf, 64);
  OP_RESULT result = OPR_OK;
	
	BYTE* data = nvs_get_data();
	const SIZETYPE size = nvs_size();

	mi_memset(data, 0xFF, size + 4);
	CHECK (mi_memequal(data, size + 4, 0xFF));
	
  nvs_erase();
	const SIZETYPE TEST_SIZE = 128;
	th_fillin_inc_bytes(data, TEST_SIZE);
	CHECK (mi_memequal(data + TEST_SIZE, size + 4 - TEST_SIZE, 0xFF)); //(size - TEST_SIZE)
	
	CHECK (OPR_OK == nvs_store());
	CHECK (mi_memequal(data + TEST_SIZE, (size - TEST_SIZE), 0xFF));
	CHECK (OPR_OK == nvs_load());
	CHECK (mi_memequal(data + TEST_SIZE, (size - TEST_SIZE), 0xFF)); 
	CHECK (th_verify_inc_bytes(data, TEST_SIZE));
	
  return result;
}



OP_RESULT nvs_test_all(void) {

  OP_RESULT r;
  const uint32_t ITERS = 1;


	dc_print("Start nvs_read_test...");
  DO_TEST(nvs_read_test, ITERS);
  dc_print("Done; \n");
	
	dc_print("Start nvs_verify_test...");
  DO_TEST(nvs_verify_test, ITERS);
  dc_print("Done; \n");
	
	dc_print("Start nvs_erase_test...");
  DO_TEST(nvs_erase_test, ITERS);
  dc_print("Done; \n");
	
	dc_print("Start nvs_store_test...");
  DO_TEST(nvs_store_test, ITERS);
  dc_print("Done; \n");
	

  return OPR_OK;
}

