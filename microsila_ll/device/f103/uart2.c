
#include "hw_desc.h"
#include "uart2.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "../../core/utils.h"
#include "../../periph/uarts_priv.h"

static MiUartDescriptor uart2 = {0};

static LL_USART_InitTypeDef USART2_InitStruct = {0};
static SIZETYPE uart2_last_dmarx_index = 0; // writer index of _uart2_dma_rxbuf

// backup rx arrays
#ifdef UART2_USE_RX
static BYTE _uart2_dma_uart2_rxbuf_backup_arr[UART2_DMA_RX_BUF_SIZE];
static BYTE _uart2_prim_uart2_rxbuf_backup_arr[UART2_PRIM_RX_BUF_SIZE + sizeof(ByteBuf)];
static BYTE _uart2_sec_uart2_rxbuf_backup_arr[UART2_SEC_RX_BUF_SIZE + sizeof(ByteBuf)];
static BYTE _uart2_rxbuf_backup_arr[UART2_RX_BUF_SIZE + sizeof(ByteBuf)];
#endif

// backup tx arrays
#ifdef UART2_USE_TX
static BYTE _uart2_dma_uart2_txbuf_backup_arr[UART2_DMA_TX_BUF_SIZE];
static BYTE _uart2_txbuf_backup_arr[UART2_TX_BUF_SIZE + sizeof(ByteBuf)];
#endif

#ifndef _DISABLE_INTERRUPTS
#define _DISABLE_INTERRUPTS  \
	uint32_t _old_primask = __get_PRIMASK(); \
	__disable_irq();

#define _RESTORE_INTERRUPTS  __set_PRIMASK(_old_primask);
#endif

#define _UART2_CLEAR_TX_INT_FLAGS \
			UART2_DMA_CLEAR_TX_HT_FLAG; \
			UART2_DMA_CLEAR_TX_TC_FLAG; \
			UART2_DMA_CLEAR_TX_GI_FLAG; \
			UART2_DMA_CLEAR_TX_TE_FLAG;
			
#define _UART2_RESET_TX_BUFFERS \
			uart2.txbuf = bb_init(_uart2_txbuf_backup_arr, UART2_TX_BUF_SIZE); \
			uart2._dma_txbuf = _uart2_dma_uart2_txbuf_backup_arr; \
			uart2.txbuf_free_space = UART2_TX_BUF_SIZE;

#define _UART2_CLEAR_RX_INT_FLAGS \
			UART2_DMA_CLEAR_RX_HT_FLAG; \
			UART2_DMA_CLEAR_RX_TC_FLAG; \
			UART2_DMA_CLEAR_RX_GI_FLAG; \
			UART2_DMA_CLEAR_RX_TE_FLAG; \
			LL_USART_ClearFlag_PE(USART2); \
			LL_USART_ClearFlag_IDLE(USART2); \
			LL_USART_ClearFlag_RXNE(USART2); \
			LL_USART_ClearFlag_TC(USART2); \
			LL_USART_ClearFlag_LBD(USART2); \
			LL_USART_ClearFlag_nCTS(USART2);

			
#define _UART2_RESET_RX_BUFFERS \
			uart2._dma_rxbuf = _uart2_dma_uart2_rxbuf_backup_arr; \
			uart2.rxbuf = bb_init(_uart2_rxbuf_backup_arr, UART2_RX_BUF_SIZE); \
			uart2._prim_rxbuf = bb_init(_uart2_prim_uart2_rxbuf_backup_arr, UART2_PRIM_RX_BUF_SIZE); \
			uart2._prim_rxbuf_locked = FALSE; \
			uart2._sec_rxbuf = bb_init(_uart2_sec_uart2_rxbuf_backup_arr, UART2_SEC_RX_BUF_SIZE); \
			uart2_last_dmarx_index = 0;



void uart2_init(uint32_t baud_rate, 
							  void(*data_rx_callback)(ByteBuf* rxdata),
								const uint8_t dma_rx_int_priority,
								const uint8_t dma_tx_int_priority,
								const uint8_t uart_int_priority) {
	// Common part
	// Enable periferial clock for USART, DMA and GPIO
	UART2_ENABLE_CLOCK;
	UART2_ENABLE_DMA_CLOCK;
	UART2_ENABLE_GPIO_CLOCK;
	
	// Reset MiUartDescriptor									
	if (!baud_rate) { baud_rate = UART_DEFAULT_BAUD_RATE; }								
	uart2._baud = baud_rate;
	uart2._dma_rx_int_priority = dma_rx_int_priority;
	uart2._dma_tx_int_priority = dma_tx_int_priority;
	uart2._uart_int_priority = uart_int_priority;
	
	// init struct declaration
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	USART2_InitStruct.TransferDirection = LL_USART_DIRECTION_NONE;
	
	// Tx part
	#ifdef UART2_USE_TX
	// init buffers
	_UART2_RESET_TX_BUFFERS;
	uart2._tx_busy = 0;
	// reset rx callback function
	uart2.on_rx_callback = 0;
	
	// GPIO
	GPIO_InitStruct.Pin = UART2_TX_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
	LL_GPIO_Init(UART2_TX_PORT, &GPIO_InitStruct);
	
	// DMA
	LL_DMA_SetDataTransferDirection(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetChannelPriorityLevel(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_MODE_NORMAL);
	LL_DMA_SetPeriphIncMode(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(UART2_DMA, UART2_DMA_TX_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_SetPeriphAddress(UART2_DMA, UART2_DMA_TX_CHANNEL, (uint32_t)&USART2->DR);
	LL_DMA_SetMemoryAddress(UART2_DMA, UART2_DMA_TX_CHANNEL, (uint32_t)uart2._dma_txbuf);
	LL_DMA_SetDataLength(UART2_DMA, UART2_DMA_TX_CHANNEL, 0);

	// DMA_TX_IRQn interrupt configuration
	NVIC_SetPriority(UART2_DMA_TX_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 
									uart2._dma_tx_int_priority, 0));
									
	NVIC_EnableIRQ(UART2_DMA_TX_IRQn);
	
	// USART set transfer direction
	USART2_InitStruct.TransferDirection |= LL_USART_DIRECTION_TX;
	
	#endif // Tx part
	
	
	// Rx part
	#ifdef UART2_USE_RX
	
	// init buffers
	_UART2_RESET_RX_BUFFERS;
	
	// set rx callback function
	uart2.on_rx_callback = data_rx_callback;
	
	// GPIO
	GPIO_InitStruct.Pin = UART2_RX_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
	LL_GPIO_Init(UART2_RX_PORT, &GPIO_InitStruct);
	
	// DMA 
	LL_DMA_SetDataTransferDirection(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetChannelPriorityLevel(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(UART2_DMA, UART2_DMA_RX_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_SetPeriphAddress(UART2_DMA, UART2_DMA_RX_CHANNEL, (uint32_t)&USART2->DR);
	LL_DMA_SetMemoryAddress(UART2_DMA, UART2_DMA_RX_CHANNEL, (uint32_t)uart2._dma_rxbuf);
	LL_DMA_SetDataLength(UART2_DMA, UART2_DMA_RX_CHANNEL, UART2_DMA_RX_BUF_SIZE);
	
	// DMA_RX_IRQn interrupt configuration
	NVIC_SetPriority(UART2_DMA_RX_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 
									uart2._dma_rx_int_priority, 0));
	NVIC_EnableIRQ(UART2_DMA_RX_IRQn);
	
	// USART set transfer direction
	USART2_InitStruct.TransferDirection |= LL_USART_DIRECTION_RX;
	#endif // Rx part
	
	// Final common part
	USART2_InitStruct.BaudRate = uart2._baud;
	USART2_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART2_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART2_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART2_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	LL_USART_Init(USART2, &USART2_InitStruct);
	LL_USART_ConfigAsyncMode(USART2);

	// USART2 interrupt
	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
									uart2._uart_int_priority, 0));
	NVIC_EnableIRQ(USART2_IRQn);
}


void uart2_set_baud(const uint32_t baud_rate) {
	uart2._baud = baud_rate;

	#ifdef UART2_USE_TX
	uart2_tx_disable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_disable();
	#endif
	
	LL_USART_Disable(USART2);
	
	USART2_InitStruct.BaudRate = baud_rate;
	LL_USART_DeInit(USART2);
	LL_USART_Init(USART2, &USART2_InitStruct);
	
	#ifdef UART2_USE_TX
	uart2_tx_enable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_enable();
	#endif
}


inline void uart2_enable(void) {
	#ifdef UART2_USE_TX
	uart2_tx_enable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_enable();
	#endif
}


inline void uart2_disable(void) {
	#ifdef UART2_USE_TX
	uart2_tx_disable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_disable();
	#endif

	LL_USART_Disable(USART2);
}


void uart2_reset(void) {

	#ifdef UART2_USE_TX
	uart2_tx_disable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_disable();
	#endif
	
	LL_USART_Disable(USART2);
	LL_USART_DeInit(USART2);
	LL_USART_Init(USART2, &USART2_InitStruct);
	
	#ifdef UART2_USE_TX
	uart2_tx_enable();
	#endif
	
	#ifdef UART2_USE_RX
	uart2_rx_enable();
	#endif
}


#ifdef UART2_USE_RX
void uart2_rx_enable(void) {
	_DISABLE_INTERRUPTS;
	_UART2_CLEAR_RX_INT_FLAGS;
	LL_DMA_SetMemoryAddress(UART2_DMA, UART2_DMA_RX_CHANNEL, 
													(uint32_t)uart2._dma_rxbuf);
	LL_DMA_SetDataLength(UART2_DMA, UART2_DMA_RX_CHANNEL, UART2_DMA_RX_BUF_SIZE);
	LL_DMA_EnableIT_HT(UART2_DMA, UART2_DMA_RX_CHANNEL); // Half transfer complete interrupt
  LL_DMA_EnableIT_TC(UART2_DMA, UART2_DMA_RX_CHANNEL); // Transfer complete interrupt
	LL_USART_EnableIT_IDLE(USART2);  // Idle interrupt
	LL_USART_EnableDMAReq_RX(USART2);
	LL_DMA_EnableChannel(UART2_DMA, UART2_DMA_RX_CHANNEL); // DMA
	LL_USART_Enable(USART2);  // Enable USART2
	while (!LL_DMA_IsEnabledChannel(UART2_DMA, UART2_DMA_RX_CHANNEL)) { }
	_RESTORE_INTERRUPTS;
	_UART2_RESET_RX_BUFFERS;
}


void uart2_rx_disable(void) {
	_DISABLE_INTERRUPTS;
	LL_DMA_DisableIT_HT(UART2_DMA, UART2_DMA_RX_CHANNEL); // Half transfer complete interrupt
  LL_DMA_DisableIT_TC(UART2_DMA, UART2_DMA_RX_CHANNEL); // Transfer complete interrupt
	LL_USART_DisableIT_IDLE(USART2);  // Idle interrupt
	LL_USART_DisableDMAReq_RX(USART2);
	LL_DMA_DisableChannel(UART2_DMA, UART2_DMA_RX_CHANNEL); // DMA
	_UART2_CLEAR_RX_INT_FLAGS;
	_RESTORE_INTERRUPTS;
	_UART2_RESET_RX_BUFFERS;
}


static inline void _uart2_append_rx_data_securely(const BYTE* data, SIZETYPE size) {
	if (!uart2._prim_rxbuf_locked) { 
			// write received data into prim_rxbuf if there is enough free space in it
			bb_append_array_if_fits(uart2._prim_rxbuf, data, size);
		} else { // write received data to sec_rxbuf if there is enough free space in it
			bb_append_array_if_fits(uart2._sec_rxbuf, data, size);
		}
}


void uart2_rx_interrupt_common_handler(void) {
	SIZETYPE index;
	// calculate current position in buffer 
	// ! LL_DMA_GetDataLength() returns the remaining number of bytes to be transfered by DMA,
	// it's a kind of a backward counter
	index = UART2_DMA_RX_BUF_SIZE - LL_DMA_GetDataLength(UART2_DMA, UART2_DMA_RX_CHANNEL);
	if (index != uart2_last_dmarx_index) { // if data received
		SIZETYPE size;
		BYTE* from;
		if (index > uart2_last_dmarx_index) { // one data chunk from last_index to index
			size = index - uart2_last_dmarx_index;
			from = uart2._dma_rxbuf + uart2_last_dmarx_index;
			_uart2_append_rx_data_securely(from, size);
		} else { // pos < old_pos
			// two data chunks: from last_index to buffer_end,
			// from buffer_start to index
			size = UART2_DMA_RX_BUF_SIZE - uart2_last_dmarx_index;
			from = uart2._dma_rxbuf + uart2_last_dmarx_index;
			_uart2_append_rx_data_securely(from, size);
			size = index;
			from = uart2._dma_rxbuf;
			_uart2_append_rx_data_securely(from, size);
		}
	}
	// reset index if it is end of buffer
	if (index == UART2_DMA_RX_BUF_SIZE) {
		index = 0;
	}
	uart2_last_dmarx_index = index; // save current index as last index
}
		

inline void uart2_isr(void) {
	if (LL_USART_IsEnabledIT_IDLE(USART2) && LL_USART_IsActiveFlag_IDLE(USART2)) {
		LL_USART_ClearFlag_IDLE(USART2); // clear IDLE line flag
		uart2_rx_interrupt_common_handler();
	}
}


inline void uart2_dma_rx_isr(void) {
		// check half-transfer complete interrupt
    if (LL_DMA_IsEnabledIT_HT(UART2_DMA, UART2_DMA_RX_CHANNEL) 
															&& UART2_DMA_RX_HT_FLAG_ACTIVE) {
        UART2_DMA_CLEAR_RX_HT_FLAG;             // clear half-transfer complete flag
				uart2_rx_interrupt_common_handler();
    }

    // check transfer-complete interrupt
    if (LL_DMA_IsEnabledIT_TC(UART2_DMA, UART2_DMA_RX_CHANNEL) 
															&& UART2_DMA_RX_TC_FLAG_ACTIVE) {
        UART2_DMA_CLEAR_RX_TC_FLAG;             // clear transfer complete flag
        uart2_rx_interrupt_common_handler();
    }
}


void uart2_do_processing(void) {
	uart2._prim_rxbuf_locked = TRUE;
	bb_transfer(uart2._prim_rxbuf, uart2.rxbuf);
	bb_compact(uart2._prim_rxbuf);
	uart2._prim_rxbuf_locked = FALSE;
	bb_transfer(uart2._sec_rxbuf, uart2.rxbuf);
	bb_compact(uart2._sec_rxbuf);
	
	if (!bb_empty(uart2.rxbuf)) { // if there is received data
		if (uart2.on_rx_callback) {
			uart2.on_rx_callback(uart2.rxbuf);
		}
		// discard all unprocessed data
		bb_reset(uart2.rxbuf);
	}
}

#endif // UART2_USE_RX


#ifdef UART2_USE_TX

void uart2_tx_enable(void) {
	_UART2_RESET_TX_BUFFERS;
	_DISABLE_INTERRUPTS;
	_UART2_CLEAR_TX_INT_FLAGS;
	LL_DMA_EnableIT_TC(UART2_DMA, UART2_DMA_TX_CHANNEL); 
	LL_DMA_EnableIT_TE(UART2_DMA, UART2_DMA_TX_CHANNEL); 
	LL_USART_EnableDMAReq_TX(USART2);
	LL_DMA_SetDataLength(UART2_DMA, UART2_DMA_TX_CHANNEL, 0);
	LL_DMA_SetMemoryAddress(UART2_DMA, UART2_DMA_TX_CHANNEL, 
													(uint32_t) uart2._dma_txbuf);
	
	LL_DMA_EnableChannel(UART2_DMA, UART2_DMA_TX_CHANNEL);
	while (!LL_DMA_IsEnabledChannel(UART2_DMA, UART2_DMA_TX_CHANNEL)) { }
	LL_USART_Enable(USART2);  
	uart2._tx_busy = FALSE;
	_RESTORE_INTERRUPTS;
}


void uart2_tx_disable(void) {
	LL_DMA_DisableIT_TC(UART2_DMA, UART2_DMA_TX_CHANNEL);
	LL_DMA_DisableIT_TE(UART2_DMA, UART2_DMA_TX_CHANNEL); 
	LL_USART_DisableDMAReq_TX(USART2);
	LL_DMA_DisableChannel(UART2_DMA, UART2_DMA_TX_CHANNEL);
	if (uart2._tx_busy) {
		// wait until current transfer is complete
		while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
		uart2._tx_busy = FALSE;
	}
	_UART2_CLEAR_TX_INT_FLAGS;
	_UART2_RESET_TX_BUFFERS;
}


inline BOOL uart2_txbuf_can_fit(const SIZETYPE size) {
	return (uart2.txbuf_free_space >= size);
}


// Start DMA transmission if there is some data to transmit;
// this function may be called from both main() thread and DMA TX interrupt,
// the latter must also set _tx_busy to FALSE before that.
void _start_uart2_tx(void) {
	if (uart2._tx_busy) { return; }  // already started
	
	_DISABLE_INTERRUPTS;
	
	// disable DMA tx channel if enabled 
	LL_DMA_DisableChannel(UART2_DMA, UART2_DMA_TX_CHANNEL);

	_UART2_CLEAR_TX_INT_FLAGS;
	
	// check if there is data to transmit
	SIZETYPE size = bb_unread_size(uart2.txbuf);
	if (!size) { _RESTORE_INTERRUPTS; return; }
	
	// ensure that data chunk to be tranmitted is not bigger than DMA buffer
	if (size > UART2_DMA_TX_BUF_SIZE) { size = UART2_DMA_TX_BUF_SIZE; }

	// copy portion of data into dma_tx_buf
	bb_read_into_array(uart2.txbuf, uart2._dma_txbuf, size);
	// update free space counter so that main() thread can easily estimate
	// if data can be enqueued into txbuf
	uart2.txbuf_free_space += size;
	
	// start DMA transfer 
	LL_DMA_SetDataLength(UART2_DMA, UART2_DMA_TX_CHANNEL, size);
	LL_DMA_SetMemoryAddress(UART2_DMA, UART2_DMA_TX_CHANNEL, (uint32_t) uart2._dma_txbuf);
	uart2._tx_busy = TRUE; 
	
	// start new transfer
	LL_DMA_EnableChannel(UART2_DMA, UART2_DMA_TX_CHANNEL);
	_RESTORE_INTERRUPTS;
}


inline void uart2_dma_tx_isr(void) {
	if (LL_DMA_IsEnabledIT_TC(UART2_DMA, UART2_DMA_TX_CHANNEL) 
		              && UART2_DMA_TX_TC_FLAG_ACTIVE) {
		UART2_DMA_CLEAR_TX_TC_FLAG;		
		uart2._tx_busy = FALSE;
		_start_uart2_tx();  // start tx of new data portion (if any) from uart2.txbuf
	}
									
	if (UART2_DMA_TX_TE_FLAG_ACTIVE) {
		UART2_DMA_CLEAR_TX_TE_FLAG; // clear transfer error flag 
		// process error if required
	}
}


OP_RESULT uart2_tx(const BYTE* data, const SIZETYPE size) {
	if (!size) { return OPR_OK; }
	// ensure no DMA interrupts occur while modifying uart2.txbuf
	_DISABLE_INTERRUPTS;
	// check if there is enough space in the txbuf
	SIZETYPE freed = bb_lazy_compact(uart2.txbuf, size);
	if (size > bb_free_space(uart2.txbuf)) {
		_RESTORE_INTERRUPTS;
		return OPR_OUT_OF_SPACE;
	}
	
	// write data into the txbuf
	if (OPR_OK == bb_append_array_if_fits(uart2.txbuf, data, size)) {
		// update txbuf free space counter
		uart2.txbuf_free_space -= size;
	}
	_RESTORE_INTERRUPTS;
	_start_uart2_tx(); // ensure tx sequence started
	return OPR_OK;
}


void uart2_tx_all(const BYTE* data, const SIZETYPE size) {
	SIZETYPE bytes_transmitted = 0; 
	SIZETYPE chunk_size;
	while (bytes_transmitted < size) {
		chunk_size = size - bytes_transmitted;
		if (chunk_size > UART2_TX_BUF_SIZE) { chunk_size = UART2_TX_BUF_SIZE; }
		// wait until there is enough space in the tx buf
		while (!uart2_txbuf_can_fit(chunk_size)) { } 
		uart2_tx(data, chunk_size);
		bytes_transmitted += chunk_size;
		data += chunk_size;
	}
}


inline void uart2_tx_str(const char* strzero) {
	SIZETYPE len = mi_strlen(strzero);
	uart2_tx_all((const BYTE*) strzero, len);
}


inline SIZETYPE uart2_tx_buf(ByteBuf* data) {
	SIZETYPE bytes_to_tx = bb_unread_size(data);
	if (bytes_to_tx > uart2.txbuf_free_space) {
		bytes_to_tx = uart2.txbuf_free_space;
	}
	OP_RESULT result =  uart2_tx(data->data, bytes_to_tx);
	if (OPR_OK == result) { data->rIndex += bytes_to_tx; }
	return bytes_to_tx;
}


inline void uart2_tx_buf_all(ByteBuf* data) {
	SIZETYPE size = bb_unread_size(data);
	uart2_tx_all(data->data, size);
	data->rIndex += size;
}


#endif  // UART2_USE_TX
