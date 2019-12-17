
#include "hw_desc.h"
#include "uart3.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "../../core/utils.h"
#include "../../periph/uarts_priv.h"

static MiUartDescriptor uart3 = {0};

static LL_USART_InitTypeDef USART3_InitStruct = {0};
static SIZETYPE uart3_last_dmarx_index = 0; // writer index of _uart3_dma_rxbuf

// backup rx arrays
#ifdef UART3_USE_RX
static BYTE _uart3_dma_uart3_rxbuf_backup_arr[UART3_DMA_RX_BUF_SIZE];
static BYTE _uart3_prim_uart3_rxbuf_backup_arr[UART3_PRIM_RX_BUF_SIZE + sizeof(ByteBuf)];
static BYTE _uart3_sec_uart3_rxbuf_backup_arr[UART3_SEC_RX_BUF_SIZE + sizeof(ByteBuf)];
static BYTE _uart3_rxbuf_backup_arr[UART3_RX_BUF_SIZE + sizeof(ByteBuf)];
#endif

// backup tx arrays
#ifdef UART3_USE_TX
static BYTE _uart3_dma_uart3_txbuf_backup_arr[UART3_DMA_TX_BUF_SIZE];
static BYTE _uart3_txbuf_backup_arr[UART3_TX_BUF_SIZE + sizeof(ByteBuf)];
#endif

#ifndef _DISABLE_INTERRUPTS
#define _DISABLE_INTERRUPTS  \
	uint32_t _old_primask = __get_PRIMASK(); \
	__disable_irq();

#define _RESTORE_INTERRUPTS  __set_PRIMASK(_old_primask);
#endif

#define _UART3_CLEAR_TX_INT_FLAGS \
			UART3_DMA_CLEAR_TX_HT_FLAG; \
			UART3_DMA_CLEAR_TX_TC_FLAG; \
			UART3_DMA_CLEAR_TX_GI_FLAG; \
			UART3_DMA_CLEAR_TX_TE_FLAG;
			
#define _UART3_RESET_TX_BUFFERS \
			uart3.txbuf = bb_init(_uart3_txbuf_backup_arr, UART3_TX_BUF_SIZE); \
			uart3._dma_txbuf = _uart3_dma_uart3_txbuf_backup_arr; \
			uart3.txbuf_free_space = UART3_TX_BUF_SIZE;

#define _UART3_CLEAR_RX_INT_FLAGS \
			UART3_DMA_CLEAR_RX_HT_FLAG; \
			UART3_DMA_CLEAR_RX_TC_FLAG; \
			UART3_DMA_CLEAR_RX_GI_FLAG; \
			UART3_DMA_CLEAR_RX_TE_FLAG; \
			LL_USART_ClearFlag_PE(USART3); \
			LL_USART_ClearFlag_IDLE(USART3); \
			LL_USART_ClearFlag_RXNE(USART3); \
			LL_USART_ClearFlag_TC(USART3); \
			LL_USART_ClearFlag_LBD(USART3); \
			LL_USART_ClearFlag_nCTS(USART3);

			
#define _UART3_RESET_RX_BUFFERS \
			uart3._dma_rxbuf = _uart3_dma_uart3_rxbuf_backup_arr; \
			uart3.rxbuf = bb_init(_uart3_rxbuf_backup_arr, UART3_RX_BUF_SIZE); \
			uart3._prim_rxbuf = bb_init(_uart3_prim_uart3_rxbuf_backup_arr, UART3_PRIM_RX_BUF_SIZE); \
			uart3._prim_rxbuf_locked = FALSE; \
			uart3._sec_rxbuf = bb_init(_uart3_sec_uart3_rxbuf_backup_arr, UART3_SEC_RX_BUF_SIZE); \
			uart3_last_dmarx_index = 0;



void uart3_init(uint32_t baud_rate, 
							  void(*data_rx_callback)(ByteBuf* rxdata),
								const uint8_t dma_rx_int_priority,
								const uint8_t dma_tx_int_priority,
								const uint8_t uart_int_priority) {
	// Common part
	// Enable periferial clock for USART, DMA and GPIO
	UART3_ENABLE_CLOCK;
	UART3_ENABLE_DMA_CLOCK;
	UART3_ENABLE_GPIO_CLOCK;
	
	// Reset MiUartDescriptor									
	if (!baud_rate) { baud_rate = UART_DEFAULT_BAUD_RATE; }								
	uart3._baud = baud_rate;
	uart3._dma_rx_int_priority = dma_rx_int_priority;
	uart3._dma_tx_int_priority = dma_tx_int_priority;
	uart3._uart_int_priority = uart_int_priority;
	
	// init struct declaration
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	USART3_InitStruct.TransferDirection = LL_USART_DIRECTION_NONE;
	
	// Tx part
	#ifdef UART3_USE_TX
	// init buffers
	_UART3_RESET_TX_BUFFERS;
	uart3._tx_busy = 0;
	// reset rx callback function
	uart3.on_rx_callback = 0;
	
	// GPIO
	GPIO_InitStruct.Pin = UART3_TX_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
	LL_GPIO_Init(UART3_TX_PORT, &GPIO_InitStruct);
	
	// DMA
	LL_DMA_SetDataTransferDirection(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_SetChannelPriorityLevel(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_MODE_NORMAL);
	LL_DMA_SetPeriphIncMode(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(UART3_DMA, UART3_DMA_TX_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_SetPeriphAddress(UART3_DMA, UART3_DMA_TX_CHANNEL, (uint32_t)&USART3->DR);
	LL_DMA_SetMemoryAddress(UART3_DMA, UART3_DMA_TX_CHANNEL, (uint32_t)uart3._dma_txbuf);
	LL_DMA_SetDataLength(UART3_DMA, UART3_DMA_TX_CHANNEL, 0);

	// DMA_TX_IRQn interrupt configuration
	NVIC_SetPriority(UART3_DMA_TX_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 
									uart3._dma_tx_int_priority, 0));
									
	NVIC_EnableIRQ(UART3_DMA_TX_IRQn);
	
	// USART set transfer direction
	USART3_InitStruct.TransferDirection |= LL_USART_DIRECTION_TX;
	
	#endif // Tx part
	
	
	// Rx part
	#ifdef UART3_USE_RX
	
	// init buffers
	_UART3_RESET_RX_BUFFERS;
	
	// set rx callback function
	uart3.on_rx_callback = data_rx_callback;
	
	// GPIO
	GPIO_InitStruct.Pin = UART3_RX_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
	LL_GPIO_Init(UART3_RX_PORT, &GPIO_InitStruct);
	
	// DMA 
	LL_DMA_SetDataTransferDirection(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetChannelPriorityLevel(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_PRIORITY_LOW);
	LL_DMA_SetMode(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_MODE_CIRCULAR);
	LL_DMA_SetPeriphIncMode(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_MEMORY_INCREMENT);
	LL_DMA_SetPeriphSize(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(UART3_DMA, UART3_DMA_RX_CHANNEL, LL_DMA_MDATAALIGN_BYTE);
	LL_DMA_SetPeriphAddress(UART3_DMA, UART3_DMA_RX_CHANNEL, (uint32_t)&USART3->DR);
	LL_DMA_SetMemoryAddress(UART3_DMA, UART3_DMA_RX_CHANNEL, (uint32_t)uart3._dma_rxbuf);
	LL_DMA_SetDataLength(UART3_DMA, UART3_DMA_RX_CHANNEL, UART3_DMA_RX_BUF_SIZE);
	
	// DMA_RX_IRQn interrupt configuration
	NVIC_SetPriority(UART3_DMA_RX_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 
									uart3._dma_rx_int_priority, 0));
	NVIC_EnableIRQ(UART3_DMA_RX_IRQn);
	
	// USART set transfer direction
	USART3_InitStruct.TransferDirection |= LL_USART_DIRECTION_RX;
	#endif // Rx part
	
	// Final common part
	USART3_InitStruct.BaudRate = uart3._baud;
	USART3_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART3_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART3_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART3_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	LL_USART_Init(USART3, &USART3_InitStruct);
	LL_USART_ConfigAsyncMode(USART3);

	// USART3 interrupt
	NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
									uart3._uart_int_priority, 0));
	NVIC_EnableIRQ(USART3_IRQn);
}


void uart3_set_baud(const uint32_t baud_rate) {
	uart3._baud = baud_rate;

	#ifdef UART3_USE_TX
	uart3_tx_disable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_disable();
	#endif
	
	LL_USART_Disable(USART3);
	
	USART3_InitStruct.BaudRate = baud_rate;
	LL_USART_DeInit(USART3);
	LL_USART_Init(USART3, &USART3_InitStruct);
	
	#ifdef UART3_USE_TX
	uart3_tx_enable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_enable();
	#endif
}


inline void uart3_enable(void) {
	#ifdef UART3_USE_TX
	uart3_tx_enable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_enable();
	#endif
}


inline void uart3_disable(void) {
	#ifdef UART3_USE_TX
	uart3_tx_disable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_disable();
	#endif

	LL_USART_Disable(USART3);
}


void uart3_reset(void) {

	#ifdef UART3_USE_TX
	uart3_tx_disable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_disable();
	#endif
	
	LL_USART_Disable(USART3);
	LL_USART_DeInit(USART3);
	LL_USART_Init(USART3, &USART3_InitStruct);
	
	#ifdef UART3_USE_TX
	uart3_tx_enable();
	#endif
	
	#ifdef UART3_USE_RX
	uart3_rx_enable();
	#endif
}


#ifdef UART3_USE_RX
void uart3_rx_enable(void) {
	_DISABLE_INTERRUPTS;
	_UART3_CLEAR_RX_INT_FLAGS;
	LL_DMA_SetMemoryAddress(UART3_DMA, UART3_DMA_RX_CHANNEL, 
													(uint32_t)uart3._dma_rxbuf);
	LL_DMA_SetDataLength(UART3_DMA, UART3_DMA_RX_CHANNEL, UART3_DMA_RX_BUF_SIZE);
	LL_DMA_EnableIT_HT(UART3_DMA, UART3_DMA_RX_CHANNEL); // Half transfer complete interrupt
  LL_DMA_EnableIT_TC(UART3_DMA, UART3_DMA_RX_CHANNEL); // Transfer complete interrupt
	LL_USART_EnableIT_IDLE(USART3);  // Idle interrupt
	LL_USART_EnableDMAReq_RX(USART3);
	LL_DMA_EnableChannel(UART3_DMA, UART3_DMA_RX_CHANNEL); // DMA
	LL_USART_Enable(USART3);  // Enable USART3
	while (!LL_DMA_IsEnabledChannel(UART3_DMA, UART3_DMA_RX_CHANNEL)) { }
	_RESTORE_INTERRUPTS;
	_UART3_RESET_RX_BUFFERS;
}


void uart3_rx_disable(void) {
	_DISABLE_INTERRUPTS;
	LL_DMA_DisableIT_HT(UART3_DMA, UART3_DMA_RX_CHANNEL); // Half transfer complete interrupt
  LL_DMA_DisableIT_TC(UART3_DMA, UART3_DMA_RX_CHANNEL); // Transfer complete interrupt
	LL_USART_DisableIT_IDLE(USART3);  // Idle interrupt
	LL_USART_DisableDMAReq_RX(USART3);
	LL_DMA_DisableChannel(UART3_DMA, UART3_DMA_RX_CHANNEL); // DMA
	_UART3_CLEAR_RX_INT_FLAGS;
	_RESTORE_INTERRUPTS;
	_UART3_RESET_RX_BUFFERS;
}


static inline void _uart3_append_rx_data_securely(const BYTE* data, SIZETYPE size) {
	if (!uart3._prim_rxbuf_locked) { 
			// write received data into prim_rxbuf if there is enough free space in it
			bb_append_array_if_fits(uart3._prim_rxbuf, data, size);
		} else { // write received data to sec_rxbuf if there is enough free space in it
			bb_append_array_if_fits(uart3._sec_rxbuf, data, size);
		}
}


void uart3_rx_interrupt_common_handler(void) {
	SIZETYPE index;
	// calculate current position in buffer 
	// ! LL_DMA_GetDataLength() returns the remaining number of bytes to be transfered by DMA,
	// it's a kind of a backward counter
	index = UART3_DMA_RX_BUF_SIZE - LL_DMA_GetDataLength(UART3_DMA, UART3_DMA_RX_CHANNEL);
	if (index != uart3_last_dmarx_index) { // if data received
		SIZETYPE size;
		BYTE* from;
		if (index > uart3_last_dmarx_index) { // one data chunk from last_index to index
			size = index - uart3_last_dmarx_index;
			from = uart3._dma_rxbuf + uart3_last_dmarx_index;
			_uart3_append_rx_data_securely(from, size);
		} else { // pos < old_pos
			// two data chunks: from last_index to buffer_end,
			// from buffer_start to index
			size = UART3_DMA_RX_BUF_SIZE - uart3_last_dmarx_index;
			from = uart3._dma_rxbuf + uart3_last_dmarx_index;
			_uart3_append_rx_data_securely(from, size);
			size = index;
			from = uart3._dma_rxbuf;
			_uart3_append_rx_data_securely(from, size);
		}
	}
	// reset index if it is end of buffer
	if (index == UART3_DMA_RX_BUF_SIZE) {
		index = 0;
	}
	uart3_last_dmarx_index = index; // save current index as last index
}
		

inline void uart3_isr(void) {
	if (LL_USART_IsEnabledIT_IDLE(USART3) && LL_USART_IsActiveFlag_IDLE(USART3)) {
		LL_USART_ClearFlag_IDLE(USART3); // clear IDLE line flag
		uart3_rx_interrupt_common_handler();
	}
}


inline void uart3_dma_rx_isr(void) {
		// check half-transfer complete interrupt
    if (LL_DMA_IsEnabledIT_HT(UART3_DMA, UART3_DMA_RX_CHANNEL) 
															&& UART3_DMA_RX_HT_FLAG_ACTIVE) {
        UART3_DMA_CLEAR_RX_HT_FLAG;             // clear half-transfer complete flag
				uart3_rx_interrupt_common_handler();
    }

    // check transfer-complete interrupt
    if (LL_DMA_IsEnabledIT_TC(UART3_DMA, UART3_DMA_RX_CHANNEL) 
															&& UART3_DMA_RX_TC_FLAG_ACTIVE) {
        UART3_DMA_CLEAR_RX_TC_FLAG;             // clear transfer complete flag
        uart3_rx_interrupt_common_handler();
    }
}


void uart3_do_processing(void) {
	uart3._prim_rxbuf_locked = TRUE;
	bb_transfer(uart3._prim_rxbuf, uart3.rxbuf);
	bb_compact(uart3._prim_rxbuf);
	uart3._prim_rxbuf_locked = FALSE;
	bb_transfer(uart3._sec_rxbuf, uart3.rxbuf);
	bb_compact(uart3._sec_rxbuf);
	
	if (!bb_empty(uart3.rxbuf)) { // if there is received data
		if (uart3.on_rx_callback) {
			uart3.on_rx_callback(uart3.rxbuf);
		}
		// discard all unprocessed data
		bb_reset(uart3.rxbuf);
	}
}

#endif // UART3_USE_RX


#ifdef UART3_USE_TX

void uart3_tx_enable(void) {
	_UART3_RESET_TX_BUFFERS;
	_DISABLE_INTERRUPTS;
	_UART3_CLEAR_TX_INT_FLAGS;
	LL_DMA_EnableIT_TC(UART3_DMA, UART3_DMA_TX_CHANNEL); 
	LL_DMA_EnableIT_TE(UART3_DMA, UART3_DMA_TX_CHANNEL); 
	LL_USART_EnableDMAReq_TX(USART3);
	LL_DMA_SetDataLength(UART3_DMA, UART3_DMA_TX_CHANNEL, 0);
	LL_DMA_SetMemoryAddress(UART3_DMA, UART3_DMA_TX_CHANNEL, 
													(uint32_t) uart3._dma_txbuf);
	
	LL_DMA_EnableChannel(UART3_DMA, UART3_DMA_TX_CHANNEL);
	while (!LL_DMA_IsEnabledChannel(UART3_DMA, UART3_DMA_TX_CHANNEL)) { }
	LL_USART_Enable(USART3);  
	uart3._tx_busy = FALSE;
	_RESTORE_INTERRUPTS;
}


void uart3_tx_disable(void) {
	LL_DMA_DisableIT_TC(UART3_DMA, UART3_DMA_TX_CHANNEL);
	LL_DMA_DisableIT_TE(UART3_DMA, UART3_DMA_TX_CHANNEL); 
	LL_USART_DisableDMAReq_TX(USART3);
	LL_DMA_DisableChannel(UART3_DMA, UART3_DMA_TX_CHANNEL);
	if (uart3._tx_busy) {
		// wait until current transfer is complete
		while (!LL_USART_IsActiveFlag_TXE(USART3)) {}
		uart3._tx_busy = FALSE;
	}
	_UART3_CLEAR_TX_INT_FLAGS;
	_UART3_RESET_TX_BUFFERS;
}


inline BOOL uart3_txbuf_can_fit(const SIZETYPE size) {
	return (uart3.txbuf_free_space >= size);
}


// Start DMA transmission if there is some data to transmit;
// this function may be called from both main() thread and DMA TX interrupt,
// the latter must also set _tx_busy to FALSE before that.
void _start_uart3_tx(void) {
	if (uart3._tx_busy) { return; }  // already started
	
	_DISABLE_INTERRUPTS;
	
	// disable DMA tx channel if enabled 
	LL_DMA_DisableChannel(UART3_DMA, UART3_DMA_TX_CHANNEL);

	_UART3_CLEAR_TX_INT_FLAGS;
	
	// check if there is data to transmit
	SIZETYPE size = bb_unread_size(uart3.txbuf);
	if (!size) { _RESTORE_INTERRUPTS; return; }
	
	// ensure that data chunk to be tranmitted is not bigger than DMA buffer
	if (size > UART3_DMA_TX_BUF_SIZE) { size = UART3_DMA_TX_BUF_SIZE; }

	// copy portion of data into dma_tx_buf
	bb_read_into_array(uart3.txbuf, uart3._dma_txbuf, size);
	// update free space counter so that main() thread can easily estimate
	// if data can be enqueued into txbuf
	uart3.txbuf_free_space += size;
	
	// start DMA transfer 
	LL_DMA_SetDataLength(UART3_DMA, UART3_DMA_TX_CHANNEL, size);
	LL_DMA_SetMemoryAddress(UART3_DMA, UART3_DMA_TX_CHANNEL, (uint32_t) uart3._dma_txbuf);
	uart3._tx_busy = TRUE; 
	
	// start new transfer
	LL_DMA_EnableChannel(UART3_DMA, UART3_DMA_TX_CHANNEL);
	_RESTORE_INTERRUPTS;
}


inline void uart3_dma_tx_isr(void) {
	if (LL_DMA_IsEnabledIT_TC(UART3_DMA, UART3_DMA_TX_CHANNEL) 
		              && UART3_DMA_TX_TC_FLAG_ACTIVE) {
		UART3_DMA_CLEAR_TX_TC_FLAG;		
		uart3._tx_busy = FALSE;
		_start_uart3_tx();  // start tx of new data portion (if any) from uart3.txbuf
	}
									
	if (UART3_DMA_TX_TE_FLAG_ACTIVE) {
		UART3_DMA_CLEAR_TX_TE_FLAG; // clear transfer error flag 
		// process error if required
	}
}


OP_RESULT uart3_tx(const BYTE* data, const SIZETYPE size) {
	if (!size) { return OPR_OK; }
	// ensure no DMA interrupts occur while modifying uart3.txbuf
	_DISABLE_INTERRUPTS;
	// check if there is enough space in the txbuf
	SIZETYPE freed = bb_lazy_compact(uart3.txbuf, size);
	if (size > bb_free_space(uart3.txbuf)) {
		_RESTORE_INTERRUPTS;
		return OPR_OUT_OF_SPACE;
	}
	
	// write data into the txbuf
	if (OPR_OK == bb_append_array_if_fits(uart3.txbuf, data, size)) {
		// update txbuf free space counter
		uart3.txbuf_free_space -= size;
	}
	_RESTORE_INTERRUPTS;
	_start_uart3_tx(); // ensure tx sequence started
	return OPR_OK;
}


void uart3_tx_all(const BYTE* data, const SIZETYPE size) {
	SIZETYPE bytes_transmitted = 0; 
	SIZETYPE chunk_size;
	while (bytes_transmitted < size) {
		chunk_size = size - bytes_transmitted;
		if (chunk_size > UART3_TX_BUF_SIZE) { chunk_size = UART3_TX_BUF_SIZE; }
		// wait until there is enough space in the tx buf
		while (!uart3_txbuf_can_fit(chunk_size)) { } 
		uart3_tx(data, chunk_size);
		bytes_transmitted += chunk_size;
		data += chunk_size;
	}
}


inline void uart3_tx_str(const char* strzero) {
	SIZETYPE len = mi_strlen(strzero);
	uart3_tx_all((const BYTE*) strzero, len);
}


inline SIZETYPE uart3_tx_buf(ByteBuf* data) {
	SIZETYPE bytes_to_tx = bb_unread_size(data);
	if (bytes_to_tx > uart3.txbuf_free_space) {
		bytes_to_tx = uart3.txbuf_free_space;
	}
	OP_RESULT result =  uart3_tx(data->data, bytes_to_tx);
	if (OPR_OK == result) { data->rIndex += bytes_to_tx; }
	return bytes_to_tx;
}


inline void uart3_tx_buf_all(ByteBuf* data) {
	SIZETYPE size = bb_unread_size(data);
	uart3_tx_all(data->data, size);
	data->rIndex += size;
}


#endif  // UART3_USE_TX
