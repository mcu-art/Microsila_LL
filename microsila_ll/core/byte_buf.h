
/*  
  Byte buffer with reader and writer indexes.
*/


#ifndef _BYTE_BUF_H
#define _BYTE_BUF_H


#ifdef __cplusplus
extern "C" {
#endif
	
#include "data_types.h"
#include <mi_ll_settings.h>

typedef struct {
	BYTE* 		  data; /* Pointer to externally allocated buffer */
	SIZETYPE  	rIndex; /* Reader index  */
	SIZETYPE  	wIndex; /* Writer index  */
	SIZETYPE 		size;	/* Data size */
} ByteBuf;

// Macro to create a ByteBuf inside a function;
// Buffer size must not be too large,
// otherwise stack overflow may occur.
#define BB_CREATE(_BUFNAME, _BUFSIZE) \
	BYTE _BUFNAME_arr[_BUFSIZE + sizeof(ByteBuf)]; \
  ByteBuf* _BUFNAME = bb_init(_BUFNAME_arr, _BUFSIZE);


/* 	Init byte buffer; memblock must be externally allocated;
data_size: actual data size of the ByteBuf; 
           Note that it is smaller than memblock size and
           must be equal to (sizeof(memblock) - sizeof(ByteBuf))
*/
extern ByteBuf* bb_init(BYTE* memblock, const SIZETYPE data_size);

/* 	Reset readerIndex and writerIndex */
extern inline void bb_reset(ByteBuf* inst);

/* 	* @descr: Zero the entire data array regardless of the actual unread data size, 
							rIndex and wIndex.
*/
extern inline void bb_zero_data(ByteBuf* inst);

/* 	* @descr: Return TRUE if there is valid data in the ByteBuf
*/
extern inline BOOL bb_empty(ByteBuf* inst);

extern inline BYTE bb_read_byte(ByteBuf* inst);
extern inline void bb_write_byte(ByteBuf* inst, BYTE data);

// Read array of bytes into byte buffer; 
// data size must NOT be greater than unread size
extern inline void bb_read_into_array(ByteBuf* inst, BYTE* dest, const SIZETYPE size);
	
// Write array of bytes into byte buffer if there is enough space;
// return OPR_OK if success or OPR_OUT_OF_SPACE if failed
extern inline OP_RESULT bb_append_array_if_fits(ByteBuf* inst, const BYTE* src, const SIZETYPE size);

// Write byte only if there is free space in the buffer if there is enough space;
// This is useful to protect memory corruption
extern inline OP_RESULT bb_write_byte_if_fits(ByteBuf* inst, const BYTE data);

// Write array of bytes into byte buffer if there is enough space;
// return OPR_OK if success or OPR_OUT_OF_SPACE if failed
extern inline OP_RESULT bb_append_str(ByteBuf* inst, const char* strzero);

// Get size of unread data
extern inline SIZETYPE bb_unread_size(ByteBuf* inst);

// Get available free space
extern inline SIZETYPE bb_free_space(ByteBuf* inst);

/* 	* @descr: Transfer unread data from src
							into dest until destination buffer is full 
							or source buffer is empty.
							readerIndex of the src and writerIndex of dest are increased respectively.
							COMPACT is NOT applied to src!
*/
extern void bb_transfer(ByteBuf* src, ByteBuf* dest);

/* 	* @descr: Move unread data 
							to the beginning of the ByteBuf. Set readerIndex to zero, 
							writerIndex to the next element after valid data end.
		* @param: ByteBuf* inst - instance of the ByteBuf
		* @return: number of freed bytes
*/
extern SIZETYPE bb_compact(ByteBuf* inst);

// The purpose of this function is to minimize unnecessary data copying in case
// producer fills data with big chunks, while consumer reads small chunks;
// This function is better to use immediately before producer writes data to the byte buffer;
// Compact only if:
//   * rIndex and wIndex are equal (low-cost compact)
//   * available free space is less than 'min_size_to_fit' (reluctant high-cost compact)
// otherwise do not compact;
// Return: number of freed bytes
extern SIZETYPE bb_lazy_compact(ByteBuf* inst, const SIZETYPE min_size_to_fit);

#ifdef __cplusplus
}
#endif


#endif


