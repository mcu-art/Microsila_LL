
#include "test.h"
#include <microsila_ll/core/test_helpers.h>
#include <microsila_ll/core/dbg_console.h>


OP_RESULT nvs_read_test(void) {
  OP_RESULT result = OPR_OK;

  CHECK (TRUE);

  return result;
}



OP_RESULT nvs_test_all(void) {

  OP_RESULT r;
  const uint32_t ITERS = 1;

  DO_TEST(nvs_read_test, ITERS);
  
  return OPR_OK;
}

