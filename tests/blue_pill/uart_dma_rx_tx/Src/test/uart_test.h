#ifndef _UART_TEST_H
#define _UART_TEST_H

#include <mi_ll_settings.h>
#include <Microsila_LL/core/byte_buf.h>

/*  
Common preconditions:
	* Pins must be connected as follows:
			uart1 tx (PA9)  -> uart2 rx (PA3);
			uart2 tx (PA2)  -> uart3 rx (PB11);
			uart3 tx (PB10) -> uart1 rx (PA10);
			
Visualization of result:
	The STM32F103 BLUE PILL board has a LED 
	connected to PC13 which is used for visualization: 
			* success: led blinks once per second
			* timeout: led blinks fast (10 Hz)
			* error:   led is on and does not blink
*/


// Common settings
#define SRC_BUF_SIZE  	1024 * 5
#define DEST_BUF_SIZE  	1024 * 5

// Test enumeration
typedef enum {
	UART_TEST_NONE = 0,
	UART_TEST_TRIPLE_TX_RX

} UartTestId;

// Test descriptor
typedef struct {
	void* state_ptr; // pointer to test-specific TestState struct
	uint32_t start_ts; // test start time stamp
	uint32_t complete_ts; // test complete time stamp
	uint32_t deadline_ts; // deadline at which test is stopped due to timeout
	UartTestId id;
	BOOL is_complete;
  OP_RESULT result;
} UartTestDesc;

// User API

// These functions must be set as callbacks when uarts are being initialized
extern void UartTest_OnUart1DataReceived(ByteBuf* data);
extern void UartTest_OnUart2DataReceived(ByteBuf* data);
extern void UartTest_OnUart3DataReceived(ByteBuf* data);


// Init tests; must be called once;
extern OP_RESULT UartTest_Init(void);

// Convenience function to run all tests;
// this function blocks until all tests complete;
extern OP_RESULT UartTest_RunAll(void);

// Run a single test; this function blocks until test complete;
extern OP_RESULT UartTest_RunOne(const UartTestId ids);


//////////////////////////////////////////////////////////////////////////////
// Settings per test

/*  Triple tx and rx test.
Goal: check if uart1, uart2 and uart3 can transmit and receive data simultaneously
      using DMA; 6 DMA channels are engaged;
			
Description: uart1 transmits data chunks of variable length to uart2; uart2 retransmits them
      to uart3; finally, uart3 retransmits them back to uart1;
			When all data is received, it is compared with SRC; 
			
Result:			
			OPR_OK if data successfully received and verified;
			OPR_TIMEOUT if received data length is less then expected;
			OPR_ERROR if received data is not correct;

Preconditions:
			*	no special preconditions
*/

// how many bytes to transmit
#define TRIPLE_TX_RX_TEST_DATA_SIZE  			(1024 * 5)

// period in milliseconds to complete the test
#define TRIPLE_TX_RX_TEST_TIMEOUT_MILLIS  10000 


#endif
