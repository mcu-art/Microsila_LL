#include <stdio.h>



// https://stackoverflow.com/a/40063340/3824328
#define TEST_BASE_DIR tests/blue_pill/core_test/Src/test/

#define STRINGIFY_MACRO(x) STR(x)
#define STR(x) #x
#define EXPAND(x) x
#define CONCAT(n1, n2) STRINGIFY_MACRO(EXPAND(n1)EXPAND(n2))
#define CONCAT5(n1, n2, n3, n4, n5) STRINGIFY_MACRO(EXPAND(n1)EXPAND(n2)EXPAND(n3)EXPAND(n4)EXPAND(n5))

// Concatenate the five elements of your path.
// Of course, this can be simplified if there is only a prefix and a suffix
// that needs to be added and the ARCH_FAMILY, /, and ARCH are always present
// in the macro-generated #include directives.
//#include CONCAT5(LIBC_DIR,ARCH_FAMILY,/,ARCH,/stkl/printkc/printkc.h)

#include CONCAT(TEST_BASE_DIR, byte_buf_test.h)
#include CONCAT(TEST_BASE_DIR, crc_test.h)
#include CONCAT(TEST_BASE_DIR, utils_test.h)
#include CONCAT(TEST_BASE_DIR, dbg_console_test.h)
/*
#include <tests/blue_pill/core_test/Src/test/byte_buf_test.h>
#include <tests/blue_pill/core_test/Src/test/crc_test.h>
#include <tests/blue_pill/core_test/Src/test/utils_test.h>
#include <tests/blue_pill/core_test/Src/test/dbg_console_test.h>
*/


void print_result(const char* test_name, OP_RESULT result) {
    const char* res_desc;

    switch (result) {
        case OPR_OK:
            res_desc = "OK";
            break;
        case OPR_ERROR:
            res_desc = "ERROR";
            break;
        case OPR_TIMEOUT:
            res_desc = "TIMEOUT";
            break;
        default:
            res_desc = "UNKNOWN ERROR";
    }
    printf("%s: %s\n", test_name, res_desc);
}

int main()
{
   print_result("ByteBuf tests", bb_test_all());
   print_result("CRC tests", crc_test_all());
   print_result("UTILS tests", utils_test_all());
   print_result("DBG_CONSOLE tests", dbg_console_test_all());

    return 0;
}
