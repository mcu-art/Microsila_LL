/*  
*/

#include "byte_buf.h"
#include "utils.h"


ByteBuf* bb_init(BYTE* memblock, const SIZETYPE size) {
	ByteBuf* bb = (ByteBuf*) memblock;
	bb->data = memblock + sizeof(ByteBuf);
	bb->size = size;
	bb->rIndex = 0;
	bb->wIndex = 0;
	return bb;
}


inline void bb_reset(ByteBuf* inst) {
	inst->rIndex = 0;
	inst->wIndex = 0;
}


inline void bb_zero_data(ByteBuf* inst) {
	bb_reset(inst);
	mi_memzero(inst->data, inst->size); 
}


inline SIZETYPE bb_unread_size(ByteBuf* inst) {
	return inst->wIndex - inst->rIndex;
}

inline SIZETYPE bb_free_space(ByteBuf* inst) {
	return inst->size - inst->wIndex;
}

inline BOOL bb_empty(ByteBuf* i) {
	return (i->rIndex == i->wIndex) ? TRUE : FALSE;
}


inline void bb_write_byte(ByteBuf* inst, BYTE data) {
		inst->data[inst->wIndex++] = data;
}


inline void bb_read_into_array(ByteBuf* inst, BYTE* dest, const SIZETYPE size) {
	if (!size) { return; }
	BYTE* src = inst->data + inst->rIndex;
	inst->rIndex += size;
	BYTE* limit = inst->data + inst->rIndex;
	while (src != limit) *(dest++) = *(src++);
}


inline OP_RESULT bb_append_array_if_fits(ByteBuf* inst, const BYTE* src, const SIZETYPE size) {
	if (!size) { return OPR_OK; }
	if (bb_free_space(inst) < size) { return OPR_OUT_OF_SPACE; }
	BYTE* dest = inst->data + inst->wIndex;
	inst->wIndex += size;
	BYTE* limit = inst->data + inst->wIndex;
	while (dest != limit) *(dest++) = *(src++);
	return OPR_OK;
}


inline OP_RESULT bb_write_byte_if_fits(ByteBuf* inst, const BYTE data) {
	if (inst->wIndex < inst->size) { // overflow protection
		inst->data[inst->wIndex++] = data;
		return OPR_OK;
	}
	return OPR_OUT_OF_SPACE;
}

inline OP_RESULT bb_append_str(ByteBuf* inst, const char* strzero) {
	SIZETYPE len = mi_strlen(strzero);
	return bb_append_array_if_fits(inst, (const BYTE*) strzero, len);
}


inline BYTE bb_read_byte(ByteBuf* inst) {
	return inst->data[inst->rIndex++];
}


void bb_transfer(ByteBuf* src, ByteBuf* dest) {
	//Get the number of bytes to be transfered
	SIZETYPE size = bb_unread_size(src); 
	//The number cannot be greater than free space in dest buffer
	SIZETYPE free_space = dest->size - dest->wIndex;
	if (size > free_space) { size = free_space; }
	//Return if src buffer is empty or dest buffer is full
	if (!size) return;
	
  // copy data from src to dest
	BYTE* reader = src->data + src->rIndex;
	BYTE* writer = dest->data + dest->wIndex;
	BYTE* limit = reader + size;
	while (reader != limit) { *(writer++) = *(reader++); }
	// set new indexes
	src->rIndex += size;
	dest->wIndex += size;
}


SIZETYPE bb_compact(ByteBuf* inst) {
	if (!inst->rIndex) { return 0; } // already compact, nothing to do	
	// get number of bytes that haven't been read yet
	SIZETYPE size = inst->wIndex - inst->rIndex;
  SIZETYPE freed = inst->rIndex;
	if (size) { // there is some unread data
		BYTE* src = inst->data + inst->rIndex;
		BYTE* dest = inst->data;
		BYTE* limit = dest + size;
		while (dest != limit) { *(dest++) = *(src++); }
		
		// update indices
		inst->rIndex = 0;
		inst->wIndex = size;
	} else { // all data has been read, but indexes are not zero
		// reset indices 
		inst->wIndex = 0;
		inst->rIndex = 0;
	}	
	return freed;
}


SIZETYPE bb_lazy_compact(ByteBuf* inst, const SIZETYPE bytes_to_fit) {
	if (!inst->rIndex) { return 0; } // nothing to compact
	SIZETYPE freed = inst->rIndex; 
	if (inst->rIndex == inst->wIndex) { 
		bb_reset(inst); 
		return freed;
	} // low cost compact
	if (bb_free_space(inst) < bytes_to_fit) { // reluctant compact
		return bb_compact(inst);
	}
	return freed;
}
