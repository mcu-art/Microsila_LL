#ifndef _MI_LL_SETTINGS_H
#define _MI_LL_SETTINGS_H
#include <microsila_ll/core/data_types.h>

/* Hardware configuration */

/* STM32F103C8 SECTION BEGIN */
#define MI_DEVICE  STM32F103C8

/* UART SECTION  BEGIN */

// UART1
// Comment out to permanently disable UART1 receiver
#define UART1_USE_RX
// Comment out to permanently disable UART1 transmitter
#define UART1_USE_TX

/* UART rx buffer size recommendations */
// Size depends on baud rate;  64 bytes is sufficient at 115200 baud,
// 256 bytes is recommended at rates > 1Mbaud;
// The size must be divisible by 2.
// Too small size will increase CPU usage and may result in data loss;
#define  UART1_DMA_RX_BUF_SIZE  					256
// optimal size is in range (DMA_RX_BUF_SIZE, 2*DMA_RX_BUF_SIZE)
#define  UART1_PRIM_RX_BUF_SIZE  					UART1_DMA_RX_BUF_SIZE
// optimal size is half of the DMA_RX_BUF
#define  UART1_SEC_RX_BUF_SIZE  					(UART1_DMA_RX_BUF_SIZE / 2)
// depends on user needs and processing speed
#define  UART1_RX_BUF_SIZE  							(UART1_DMA_RX_BUF_SIZE * 2)

// TX
// Buffer size must be divisible by 2, too small size will increase
// CPU usage.
#define  UART1_DMA_TX_BUF_SIZE  					64
#define  UART1_TX_BUF_SIZE  		    			UART1_DMA_TX_BUF_SIZE * 2

// UART2
#define UART2_USE_RX
#define UART2_USE_TX

// RX
#define  UART2_DMA_RX_BUF_SIZE  					256
#define  UART2_PRIM_RX_BUF_SIZE  					UART2_DMA_RX_BUF_SIZE
#define  UART2_SEC_RX_BUF_SIZE  					(UART2_DMA_RX_BUF_SIZE / 2)
#define  UART2_RX_BUF_SIZE  							(UART2_DMA_RX_BUF_SIZE * 2)

#define  UART2_DMA_TX_BUF_SIZE  					64
#define  UART2_TX_BUF_SIZE  		    			UART2_DMA_TX_BUF_SIZE * 2

// UART3
#define UART3_USE_RX
#define UART3_USE_TX

// RX
#define  UART3_DMA_RX_BUF_SIZE  					256
#define  UART3_PRIM_RX_BUF_SIZE  					UART3_DMA_RX_BUF_SIZE
#define  UART3_SEC_RX_BUF_SIZE  					(UART3_DMA_RX_BUF_SIZE / 2)
#define  UART3_RX_BUF_SIZE  							(UART3_DMA_RX_BUF_SIZE * 2)

#define  UART3_DMA_TX_BUF_SIZE  					64
#define  UART3_TX_BUF_SIZE  		    			UART3_DMA_TX_BUF_SIZE * 2

/* UART SECTION  END */


/* STM32F103C8 SECTION END */

#endif

