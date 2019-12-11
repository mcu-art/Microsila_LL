#include <stdio.h>
#include <tests/blue_pill/core_test/Src/test/byte_buf_test.h>
#include <tests/blue_pill/core_test/Src/test/crc_test.h>
#include <tests/blue_pill/core_test/Src/test/utils_test.h>

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

    return 0;
}
