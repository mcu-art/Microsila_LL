#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H
#include <stdint.h>

/* Common data types; project-specific data types should be defined in `mi_ll.settings.h` */


// Operation results (note that it is not compatible with std error codes)
typedef enum {
	OPR_OK = 0,
	OPR_ERROR,
	OPR_BUSY,
	OPR_OUT_OF_SPACE,
	OPR_TIMEOUT,
	OPR_BAD_PARAMETERS,
	OPR_CRC_ERROR,
	OPR_UNDEFINED
	
} OP_RESULT;


#ifndef BYTE
#define BYTE	uint8_t
#endif


#ifndef BOOL
#define BOOL	uint8_t
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

/* Hardware configuration */

#endif

