
#include "dbg_console_test.h"
#include "microsila_ll/core/byte_buf.h"
#include "microsila_ll/core/test_helpers.h"
#include "microsila_ll/core/utils.h"
#include <stdio.h>

	
#define FIRST_PART   whi
#define SECOND_PART  le

OP_RESULT concatenating_macro_test(void) {
	OP_RESULT result = OPR_OK;
    PPCAT(FIRST_PART, SECOND_PART) (0); // Expands into while (0);
	return result;
}

OP_RESULT dc_printf_test(void) {
    OP_RESULT result = OPR_OK;

    BB_CREATE(buf, 64);
    //dc_tracebuf(buf);

    //bb_append_str(buf, "test");
    //dc_tracebuf(buf);

    //bb_reset(buf);
    //bb_append_str(buf, "This message is printed from the buffer...\n");
    //dc_printbuf(buf);

    static const char* OK = "OK";
    dc_printf("* dc_printf_test: %s * \n", OK);

    return result;
}



OP_RESULT dbg_console_test_all(void) {
	
	const uint32_t ITERS = 1;
	OP_RESULT r;

    DO_TEST(concatenating_macro_test, ITERS);
    DO_TEST(dc_printf_test, ITERS);

	return OPR_OK;
}
