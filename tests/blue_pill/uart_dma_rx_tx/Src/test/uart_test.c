
#include "uart_test.h"
#include <microsila_ll/core/utils.h>
#include <microsila_ll/periph/uarts.h>

static BYTE SRC_BUF[SRC_BUF_SIZE];
static BYTE DEST_BUF[DEST_BUF_SIZE];

UartTestDesc current;

/////////////////////////////////////////////////////////////////////////////////
// COMMON
#define TICKS_FROM_MILLIS(_MILLIS) (_MILLIS * 40)

extern volatile uint32_t systicks;


OP_RESULT UartTest_Init(void) {
	return OPR_OK;
}


static void _check_for_timeout(void) {
	if (!current.is_complete) {
		if (current.deadline_ts <= systicks) {
			current.is_complete = TRUE;
			current.result = OPR_TIMEOUT;
		}
	}
}


static void _fillin_test_descriptor(const UartTestId id, 
	                                  uint32_t timeout_millis, void* test_state) {
	current.id = id;
	current.start_ts = systicks;
	current.deadline_ts = systicks + TICKS_FROM_MILLIS(timeout_millis);
	current.complete_ts = 0;
	current.is_complete = FALSE;
	current.result = OPR_UNDEFINED;
	current.state_ptr = test_state;
}


static inline void _uarts_do_processing(void) {
	uart1_do_processing();
	uart2_do_processing();
	uart3_do_processing();
}


static void _run_test_loop(void(*TestPeriodicTask)(void)) {
	while (!current.is_complete) {
		_check_for_timeout();
		if (!current.is_complete) { 
			TestPeriodicTask();
			_uarts_do_processing();
		}
	}
}


static OP_RESULT _reset(void) {
	_fillin_test_descriptor(UART_TEST_NONE, 0, (void*) 0);
	return OPR_OK;
}




/////////////////////////////////////////////////////////////////////////////////
// TRIPLE_TX_RX TEST
typedef struct {
	uint32_t bytes_transmitted;
	uint32_t bytes_received;
	uint32_t packet_size;
} TripleTxRxTestState;


static void TripleTxRxTest_PrepareSrcData(BYTE* buf, const SIZETYPE size) {
	for (SIZETYPE i=0; i<size; ++i) {
		*(buf++) = (BYTE) i;
	}
}


static void TripleTxRxTest_DoPeriodicTask(void) {
	// Transmit chuncks of data from SRC_BUF;
	// use variable chunk length
	TripleTxRxTestState* state = (TripleTxRxTestState*) current.state_ptr;
	SIZETYPE bytes_left = TRIPLE_TX_RX_TEST_DATA_SIZE - state->bytes_transmitted;
	if (bytes_left) {
		SIZETYPE bytes_to_transmit = state->packet_size;
		if (bytes_to_transmit > TRIPLE_TX_RX_TEST_DATA_SIZE) { 
			bytes_to_transmit = TRIPLE_TX_RX_TEST_DATA_SIZE; 
		}
		if (bytes_to_transmit > bytes_left) { bytes_to_transmit = bytes_left; }
		if (uart1_txbuf_can_fit(bytes_to_transmit)) {
			uart1_tx(SRC_BUF + state->bytes_transmitted, bytes_to_transmit);
			state->bytes_transmitted += bytes_to_transmit;
			// increment tx packet size to simulate variable length
			state->packet_size++;
		}
	}
}


// Each test must define custom TestName_Run(), ..._OnUart1DataReceived(), ..._OnUart2DataReceived(),
// ..._oOnUart3DataReceived() and ..._DoPeriodicTask() functions
static OP_RESULT TripleTxRxTest_Run(void) {
	TripleTxRxTestState state;
	state.bytes_transmitted = 0;
	state.bytes_received = 0;
	state.packet_size = 1;
	
	// prepare source data
	TripleTxRxTest_PrepareSrcData(SRC_BUF, 
	                              TRIPLE_TX_RX_TEST_DATA_SIZE);
	// clear destination buffer
	mi_memzero(DEST_BUF, TRIPLE_TX_RX_TEST_DATA_SIZE);
	_fillin_test_descriptor(UART_TEST_TRIPLE_TX_RX, TRIPLE_TX_RX_TEST_TIMEOUT_MILLIS, &state);

	_run_test_loop(TripleTxRxTest_DoPeriodicTask);
	
	return current.result;
}


static void TripleTxRxTest_OnUart1DataReceived(ByteBuf* data) {
	// Read all data into the DEST_BUF, then compare it to SRC_BUF
	TripleTxRxTestState* state = (TripleTxRxTestState*) current.state_ptr;
	SIZETYPE size = bb_unread_size(data);
	bb_read_into_array(data, DEST_BUF + state->bytes_received, size);
	state->bytes_received += size;
	if (state->bytes_received == TRIPLE_TX_RX_TEST_DATA_SIZE) {
		current.complete_ts = systicks;
		current.is_complete = TRUE;
		// check that received data is correct
		BOOL correct = TRUE;
		for (SIZETYPE i=0; i<TRIPLE_TX_RX_TEST_DATA_SIZE; ++i) {
			if (DEST_BUF[i] != (BYTE) i) { correct = FALSE; break; }
		}
		if (correct) {
			current.result = OPR_OK;
		} else {
			current.result = OPR_ERROR;
		}
	}
}


static void TripleTxRxTest_OnUart2DataReceived(ByteBuf* data) {
	// forward received data to uart3
	uart2_tx_buf_all(data);
}


static void TripleTxRxTest_OnUart3DataReceived(ByteBuf* data) {
	// forward received data to uart1
	uart3_tx_buf_all(data);
}


OP_RESULT UartTest_RunOne(const UartTestId id) {
	// reset previous test
	_reset();
	
	//OP_RESULT result = OPR_ERROR;
	// run new test
	switch (id) {
		case UART_TEST_NONE:
			return _reset();
		case UART_TEST_TRIPLE_TX_RX:
			return TripleTxRxTest_Run();
	}
	
	return OPR_BAD_PARAMETERS;
}


void UartTest_OnUart1DataReceived(ByteBuf* data) {
	switch (current.id) {
		case UART_TEST_NONE:
			break;
		case UART_TEST_TRIPLE_TX_RX:
			TripleTxRxTest_OnUart1DataReceived(data);
			break;

	}
}


void UartTest_OnUart2DataReceived(ByteBuf* data) {
	switch (current.id) {
		case UART_TEST_NONE:
			break;
		case UART_TEST_TRIPLE_TX_RX:
			TripleTxRxTest_OnUart2DataReceived(data);
			break;
	}
}


void UartTest_OnUart3DataReceived(ByteBuf* data) {
	switch (current.id) {
		case UART_TEST_NONE:
			break;
		case UART_TEST_TRIPLE_TX_RX:
			TripleTxRxTest_OnUart3DataReceived(data);
			break;
	}
}


UartTestDesc* UartTest_GetResult(void) {
	return &current;
}


