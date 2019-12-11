
/* UART3 DMA rx and tx; 
Inspired by https://github.com/MaJerle/STM32_USART_DMA

*/

#ifndef _STM32F103_UART3_H
#define _STM32F103_UART3_H

#include <mi_ll_settings.h>
#include "../../core/byte_buf.h"


// Common API

// Initialize uart3; This includes DMA, GPIO, NVIC and buffer initialization;
// baud_rate: tested range is (9600, 2.1M) baud; if zero, default 115200 will be used;
// data_rx_callback: a callback function that will be called each time uart3 receives data;
// dma_rx_int_priority: number in range [0,15];
// dma_tx_int_priority: number in range [0,15];
// uart_int_priority:   number in range [0,15];
// lower number means higher priority;
extern void uart3_init(uint32_t baud_rate,
							         void(*data_rx_callback)(ByteBuf* rxdata),
											 const uint8_t dma_rx_int_priority,
								       const uint8_t dma_tx_int_priority,
								       const uint8_t uart_int_priority);

// Enable uart3 rx and tx;
// rx and tx may also be enabled separately by uart3_rx_enable()
// and uart3_tx_enable();
extern inline void uart3_enable(void);
											 
// Disable uart3 rx and tx;
// rx and tx may also be disabled separately by uart3_rx_disable()
// and uart3_tx_disable();
extern inline void uart3_disable(void);

// Stop current transmission and/or reception, reset all tx and rx buffers,
// re-enable tx and rx interrupts
extern void uart3_reset(void);

// Change baud rate, all tx and rx buffers will be reset and ongoing transmission canceled;
// For reliability, make 1..5 millisecond delay (depends on baud rate) after this and
// call uart3_reset()
extern void uart3_set_baud(const uint32_t baud_rate);

// Rx API
#ifdef UART3_USE_RX
// Clear all rx interrupt flags and buffers, enable rx
extern void uart3_rx_enable(void);

// Disable rx interrupts and rx DMA channel
extern void uart3_rx_disable(void);

// Must be periodically called from inside main() loop or on timer;
extern void uart3_do_processing(void);

// Must be called from USART_IRQHandler()
// in order to process RX IDLE interrupt
extern inline void uart3_isr(void);

// Must be called from RX_DMA_CHANNEL_IRQ_HANDLER
// in order to process RX HT, TC and TE interrupts
extern inline void uart3_dma_rx_isr(void);
#endif // UART3_USE_RX

// Tx API
#ifdef UART3_USE_TX

// Clear all tx interrupt flags and buffers, then enable tx
extern void uart3_tx_enable(void);

// Disable tx interrupts and tx DMA channel
extern void uart3_tx_disable(void);

// Quick and low-cost poll if there is enough space to put data of specified size 
// into the tx buffer
extern inline BOOL uart3_txbuf_can_fit(const SIZETYPE size);

// Transmit byte array of required size, this function doesn't block;
// If the size is greater than free space in txbuf,
// no data is copied or transmitted;
// Return value: OPR_OK if success or OPR_OUT_OF_SPACE otherwise.
// It is safe to call this function at any time from main().
extern OP_RESULT uart3_tx(const BYTE* data, const SIZETYPE size); 

// Transmit byte array of required size, all data is guaranteed to be transmitted.
// This function will block
// if the size is greater than available space in txbuf.
// It is safe to call this function at any time from main().
extern void uart3_tx_all(const BYTE* data, const SIZETYPE size); 

// Convenience function to transmit zero-terminated string;
// This function will block
// if the size is greater than available space in txbuf.
// It is safe to call at any time from main().
extern inline void uart3_tx_str(const char* strzero); 

// Convenience function to transmit data from ByteBuf;
// Returns: number of bytes transmitted.
// It is safe to call at any time from main().
extern inline SIZETYPE uart3_tx_buf(ByteBuf* data);

// Transmit data from the byte buffer, all data is guaranteed to be transmitted.
// This function will block
// if the data size is greater than available space in txbuf.
// It is safe to call this function at any time from main().
extern void uart3_tx_buf_all(ByteBuf* data); 

// Must be called from associated DMA TX interrupt handler
extern inline void uart3_dma_tx_isr(void);
#endif // UART3_USE_TX


 
#endif
