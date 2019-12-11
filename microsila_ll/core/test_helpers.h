#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H
#include <stdint.h>

/*  Macros */


#define CHECK(_STATEMENT) \
    if (!(_STATEMENT)) { result = OPR_ERROR; }

		
#define DO_TEST(_TEST, _ITERS) \
	 for (uint32_t ti=0; ti<_ITERS; ++ti) { \
		 r = _TEST(); \
		 if (r != OPR_OK) { return r; } \
	 }
	 
#endif

