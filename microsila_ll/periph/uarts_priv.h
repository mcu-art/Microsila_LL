
/* Private definitions for UARTs. */


#ifndef _UARTS_PRIV_H
#define _UARTS_PRIV_H
#include <mi_ll_settings.h>
#include "../core/byte_buf.h"

// Internal usage only
typedef struct {
	BYTE* _dma_rxbuf; // to be used from interrupt only
	BYTE* _dma_txbuf; // tx interrupts must be disabled when modifying this buffer from main()
	// double buffers allow keeping interrupts enabled while reading received data
	ByteBuf* _prim_rxbuf; // primary rx buffer
	ByteBuf* _sec_rxbuf; // secondary rx buffer
	ByteBuf* rxbuf; // receive buffer that accumulates all data
	ByteBuf* txbuf; // transmit buffer
	void(*on_rx_callback)(); // pointer to the user callback function processing received data
	volatile SIZETYPE txbuf_free_space; // how many bytes are available in the txbuf for write
	volatile BOOL _tx_busy; // TRUE while DMA transmission is in progress
	volatile BOOL _prim_rxbuf_locked; // TRUE if _prim_rxbuf is locked by main() while reading data from it
	// user assigned values
	uint32_t _baud;
	uint8_t _dma_rx_int_priority;
	uint8_t _dma_tx_int_priority;
	uint8_t _uart_int_priority;
} MiUartDescriptor;


#endif
