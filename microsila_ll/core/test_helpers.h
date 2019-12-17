#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H
#include <stdint.h>
#include <mi_ll_settings.h>

/*  Macros */


#define CHECK(_STATEMENT) \
    do { if (!(_STATEMENT)) { result = OPR_ERROR; } \
		} while (0)
    

		
#define DO_TEST(_TEST, _ITERS) \
		do { for (uint32_t ti=0; ti<_ITERS; ++ti) { \
					 r = _TEST(); \
					 if (r != OPR_OK) { return r; } \
					} \
		} \
		while (0)
	 
/* TEST HELPER FUNCTIONS BEGIN */
// Fill in the buffer with uint8_t values starting with 0 and incremented each time
extern void th_fillin_inc_bytes(BYTE* buf, const SIZETYPE size);

// Verify that the buffer contains uint8_t values starting with 0 and incremented each time
extern BOOL th_verify_inc_bytes(BYTE* buf, const SIZETYPE size);

/* TEST HELPER FUNCTIONS END */
		
#endif
