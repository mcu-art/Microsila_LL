
#include "byte_buf.h"
#include "utils.h"
#include "test_helpers.h"

void th_fillin_inc_bytes(BYTE* buf, const SIZETYPE size) {
	for (SIZETYPE i=0; i<size; ++i) {
		buf[i] = (BYTE) i;
	}
}

BOOL th_verify_inc_bytes(BYTE* buf, const SIZETYPE size) {
	for (SIZETYPE i=0; i<size; ++i) {
		if (buf[i] != (BYTE) i) { return FALSE; }
	}
	return TRUE;
}
