

#include "byte_buf_test.h"
#include "microsila_ll/core/utils.h"
#include "microsila_ll/core/test_helpers.h"

// All buffer sizes must be divisible by 4 to allign data properly
#define DATA_SIZE   300

// margins to detect data corruption
#define MARGIN_SIZE     20

// arbitrary value that helps detecting memory corruption
#define VAL  0xAE

static BYTE src_memchunk[DATA_SIZE + sizeof(ByteBuf) + 2*MARGIN_SIZE];
static BYTE* buf = src_memchunk + MARGIN_SIZE;

#define DEST_ARR_SIZE  32

static BYTE dest_arr_memchunk[DEST_ARR_SIZE + 2*MARGIN_SIZE];
static BYTE* dest_arr = dest_arr_memchunk + MARGIN_SIZE;


#define DEST_BUF_SIZE   32

static BYTE dest_buf_memchunk[DEST_BUF_SIZE + sizeof(ByteBuf) + 2*MARGIN_SIZE];
static BYTE* dest_buf= dest_buf_memchunk + MARGIN_SIZE;


static void prepare_memchunks(void) {
	mi_memset(src_memchunk, VAL, ARRLEN(src_memchunk));
	mi_memset(dest_arr_memchunk, VAL, ARRLEN(dest_arr_memchunk));
	mi_memset(dest_buf_memchunk, VAL, ARRLEN(dest_buf_memchunk));
}

static BOOL margins_not_corrupted(void) {
	// src memory chunk
	if (!mi_memequal(src_memchunk, MARGIN_SIZE, VAL)) { return FALSE; }
	if (!mi_memequal(src_memchunk + MARGIN_SIZE + DATA_SIZE + sizeof(ByteBuf), 
                   MARGIN_SIZE, VAL)) { return FALSE; }
	
	// dest_arr memory chunk
	if (!mi_memequal(dest_arr_memchunk, MARGIN_SIZE, VAL)) { return FALSE; }
	if (!mi_memequal(dest_arr_memchunk + MARGIN_SIZE + DEST_ARR_SIZE, 
									 MARGIN_SIZE, VAL)) { return FALSE; }
	
	// dest_buf memory chunk
	if (!mi_memequal(dest_buf_memchunk, MARGIN_SIZE, VAL)) { return FALSE; }
	if (!mi_memequal(dest_buf_memchunk + MARGIN_SIZE + DEST_BUF_SIZE + sizeof(ByteBuf), 
                   MARGIN_SIZE, VAL)) { return FALSE; }
	return TRUE;
}


OP_RESULT bb_init_test(void) {
	OP_RESULT result = OPR_OK;
	prepare_memchunks();
	if (!margins_not_corrupted()) { result = OPR_ERROR; }
	ByteBuf* bb = bb_init(buf, DATA_SIZE);
	CHECK(margins_not_corrupted());
	// size must be set correctly
	CHECK(bb->size == DATA_SIZE);
	// rIndex and wIndex must be correct
	CHECK((bb->rIndex == 0) && (bb->wIndex == 0));
	// all data must be set to zero - NO LONGER REQUIRED!
	// CHECK(mi_memequal(bb->data, bb->size, 0));
	// verify that there is no corruption of adjacent memory
	CHECK(margins_not_corrupted());
	
	return result;
}


OP_RESULT bb_reset_test(void) {
	OP_RESULT result = OPR_OK;
	prepare_memchunks();
	ByteBuf* bb = bb_init(buf, DATA_SIZE);
	bb->rIndex = 3;
	bb->wIndex = 5;
	CHECK (bb_unread_size(bb) == 2);
	CHECK (margins_not_corrupted());
	bb_reset(bb);
	CHECK((bb->rIndex == 0) && (bb->wIndex == 0));
	CHECK (margins_not_corrupted());
	return result;
}



static ByteBuf* _prepare_src_data(const SIZETYPE size) {
	prepare_memchunks();
	ByteBuf* bb = bb_init(buf, DATA_SIZE);
	for (SIZETYPE i=0; i<size; ++i) {
		bb_write_byte(bb, (BYTE) i);
	}
	return bb;
}

OP_RESULT bb_read_write_byte_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE);
	// check bb_empty function
	CHECK (!bb_empty(bb));
	// check bb_unread_size function
	CHECK (DATA_SIZE == bb_unread_size(bb));
	for (SIZETYPE i=0; i<DATA_SIZE; ++i) {
		// check bb_read_byte function
		BYTE d = bb_read_byte(bb);
		CHECK (d == (BYTE) i);
	}
	CHECK (0 == bb_unread_size(bb));
	CHECK (bb_empty(bb));
	// check rIndex and wIndex behave as expected
	CHECK ( (bb->rIndex == DATA_SIZE) && (bb->wIndex == DATA_SIZE));
	// check bb_reset resets indexes
	bb_reset(bb);
	CHECK ( (bb->rIndex == 0) && (bb->wIndex == 0) && (bb->size == DATA_SIZE));
	// check memory is still not corrupted
	CHECK (margins_not_corrupted());
	return result;
}


OP_RESULT bb_read_into_array_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE);

	bb_read_into_array(bb, dest_arr, DEST_ARR_SIZE);
	// check reader index
	CHECK (bb_unread_size(bb) == bb->size - DEST_ARR_SIZE);
	// check data copied correctly
	for (SIZETYPE i=0; i<DEST_ARR_SIZE; ++i) {
		CHECK (dest_arr[i] == (BYTE) i);
	}
	// check memory not corrupted
	CHECK (margins_not_corrupted());
	CHECK (bb->data[bb->wIndex] == VAL);
	CHECK (dest_arr[-1] == VAL);
	CHECK (dest_arr[DEST_ARR_SIZE] == VAL);
	return result;
}


OP_RESULT bb_append_array_if_fits_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE - 3);
	CHECK (bb_free_space(bb) == 3);
	// check memory is not corrupted when adding array lagrer than free space
	BYTE src[] = {1, 2, 3, 4};
	CHECK (OPR_OUT_OF_SPACE == bb_append_array_if_fits(bb, src, sizeof(src)));
	CHECK (bb_free_space(bb) == 3);
	CHECK (margins_not_corrupted());
	// check memory is not corrupted when adding array of same size as buffer free space
	CHECK (OPR_OK == bb_append_array_if_fits(bb, src, 3));
	CHECK (bb_free_space(bb) == 0);
	// check data correct
	CHECK (bb->data[DATA_SIZE - 1] == 3);
	CHECK (bb->data[DATA_SIZE - 2] == 2);
	CHECK (bb->data[DATA_SIZE - 3] == 1);
	CHECK (margins_not_corrupted());
	return result;
}



OP_RESULT bb_write_byte_if_fits_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE - 3);
	CHECK (bb_free_space(bb) == 3);
	CHECK (OPR_OK == bb_write_byte_if_fits(bb, 1));
	CHECK (OPR_OK == bb_write_byte_if_fits(bb, 2));
	CHECK (OPR_OK == bb_write_byte_if_fits(bb, 3));
	CHECK (OPR_OUT_OF_SPACE == bb_write_byte_if_fits(bb, 4));
	CHECK (bb->data[DATA_SIZE - 1] == 3);
	CHECK (bb->data[DATA_SIZE - 2] == 2);
	CHECK (bb->data[DATA_SIZE - 3] == 1);
	CHECK (margins_not_corrupted());
	return result;
}


OP_RESULT bb_transfer_test(void) {
	OP_RESULT result = OPR_OK;
	
	ByteBuf* src = _prepare_src_data(DATA_SIZE);
	ByteBuf* dest = bb_init(dest_buf, DEST_BUF_SIZE);
	
	bb_transfer(src, dest);
	CHECK (margins_not_corrupted());
	
	
	CHECK (bb_unread_size(src) == DATA_SIZE - DEST_BUF_SIZE);
	CHECK (bb_unread_size(dest) == DEST_BUF_SIZE);
	for (SIZETYPE i=0; i<DEST_BUF_SIZE; ++i) {
		CHECK (dest->data[i] == (BYTE) i);
	}
	
	// trying to transfer into full buffer results in nothing
	bb_transfer(src, dest);
	CHECK (margins_not_corrupted());
	CHECK (bb_unread_size(src) == DATA_SIZE - DEST_BUF_SIZE);
	CHECK (bb_unread_size(dest) == DEST_BUF_SIZE);
	
	
	bb_read_byte(dest);
	bb_compact(dest);
	bb_transfer(src, dest);
	
	CHECK (bb_unread_size(src) == (DATA_SIZE - DEST_BUF_SIZE - 1));
	CHECK (bb_unread_size(dest) == DEST_BUF_SIZE);

	CHECK (margins_not_corrupted());

	return result;
}



OP_RESULT bb_compact_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE);
	
	// when compacting full byte buffer, nothing happens
	bb_compact(bb);
	CHECK (bb_unread_size(bb) == DATA_SIZE);
	CHECK (bb_free_space(bb) == 0);
	// read one byte
	bb_read_byte(bb);
	CHECK (bb_unread_size(bb) == DATA_SIZE - 1);
	CHECK (bb_free_space(bb) == 0);
	// when compacting after data read, free space becomes available
	bb_compact(bb);
	CHECK (bb_free_space(bb) == 1);

	// check data is not corrupted after compact
	for (SIZETYPE i=0; i<DEST_BUF_SIZE - 1; ++i) {
		CHECK (bb->data[i] == (BYTE) i + 1);
	}
	
	CHECK (margins_not_corrupted());
	return result;
}


OP_RESULT bb_lazy_compact_test(void) {
	OP_RESULT result = OPR_OK;
	ByteBuf* bb = _prepare_src_data(DATA_SIZE - 3);

	
	// when lazy-compacting buffer with enough free space to
	// fit data, nothing happens
	CHECK (0 == bb_lazy_compact(bb, 3));
	
	
	bb_read_byte(bb);
	// if requered free space is equal or bigger than available free space,
	// the buffer is compacted
	CHECK (1 == bb_lazy_compact(bb, 5));
	BYTE src[] = {1, 2, 3, 4};
	CHECK (OPR_OK == bb_append_array_if_fits(bb, src, sizeof(src)));
	
	// check data is not corrupted after compact
	for (SIZETYPE i=0; i<(DATA_SIZE - 4); ++i) {
		CHECK (bb->data[i] == (BYTE) (i + 1));
	}
	
	// check newly appended data and its boundaries
	CHECK (bb->data[DATA_SIZE] == VAL);
	CHECK (bb->data[DATA_SIZE - 1] == 4);
	CHECK (bb->data[DATA_SIZE - 2] == 3);
	CHECK (bb->data[DATA_SIZE - 3] == 2);
	CHECK (bb->data[DATA_SIZE - 4] == 1);
	CHECK (bb->data[DATA_SIZE - 5] == (BYTE) (DATA_SIZE - 4));
	CHECK (margins_not_corrupted());
	return result;
}



OP_RESULT bb_test_all(void) {
	

	OP_RESULT r;
	const uint32_t ITERS = 100;

	DO_TEST(bb_transfer_test, ITERS);
	
	DO_TEST(bb_init_test, ITERS);
	DO_TEST(bb_reset_test, ITERS);
	
	
	DO_TEST(bb_read_write_byte_test, ITERS);
	 
	DO_TEST(bb_read_into_array_test, ITERS);
	 
	
	DO_TEST(bb_append_array_if_fits_test, ITERS);
	
	DO_TEST(bb_write_byte_if_fits_test, ITERS);
	
	DO_TEST(bb_transfer_test, ITERS);
	 
	DO_TEST(bb_compact_test, ITERS);
	DO_TEST(bb_lazy_compact_test, ITERS);

	return OPR_OK;
}



