#ifndef MI_LL_SETTINGS_H
#define MI_LL_SETTINGS_H
#include <microsila_ll/core/data_types.h>

/* Hardware configuration */
#define SIZETYPE uint32_t


/* DEVICE SECTION BEGIN */
#define MI_DEVICE  PC

/* UART SECTION  BEGIN */

/* Comment out UARTs that your project does not use */
//#define UART1_USE_RX
//#define UART1_USE_TX
//#define UART2_USE_RX
//#define UART2_USE_TX
//#define UART3_USE_RX
//#define UART3_USE_TX

/* Following definitions are not necessary to be commented out,
   they have no impact on memory usage if corresponding uart disabled */
/* UART rx buffer size recommendations */
// 64 bytes is sufficient at 115200 baud
// 256 bytes is recommended at rates > 1Mbaud

// Too small size will increase CPU usage and may result in data loss

// UART1 RX
#define  UART1_DMA_RX_BUF_SIZE            128
// optimal size is in range [DMA_RX_BUF_SIZE, 2 * DMA_RX_BUF_SIZE]
#define  UART1_PRIM_RX_BUF_SIZE           UART1_DMA_RX_BUF_SIZE
// optimal size is half of the DMA_RX_BUF
#define  UART1_SEC_RX_BUF_SIZE            (UART1_DMA_RX_BUF_SIZE / 2)
// depends on user needs and processing speed
#define  UART1_RX_BUF_SIZE                (UART1_DMA_RX_BUF_SIZE * 4)

// UART1 TX
#define  UART1_DMA_TX_BUF_SIZE            64
#define  UART1_TX_BUF_SIZE                UART1_DMA_TX_BUF_SIZE * 2


// UART2 RX
#define  UART2_DMA_RX_BUF_SIZE            128
#define  UART2_PRIM_RX_BUF_SIZE           UART2_DMA_RX_BUF_SIZE
#define  UART2_SEC_RX_BUF_SIZE            (UART2_DMA_RX_BUF_SIZE / 2)
#define  UART2_RX_BUF_SIZE                (UART2_DMA_RX_BUF_SIZE * 4)
// UART2 TX
#define  UART2_DMA_TX_BUF_SIZE            64
#define  UART2_TX_BUF_SIZE                UART2_DMA_TX_BUF_SIZE * 2

// UART3 RX
#define  UART3_DMA_RX_BUF_SIZE            128
#define  UART3_PRIM_RX_BUF_SIZE           UART3_DMA_RX_BUF_SIZE
#define  UART3_SEC_RX_BUF_SIZE            (UART3_DMA_RX_BUF_SIZE / 2)
#define  UART3_RX_BUF_SIZE                (UART3_DMA_RX_BUF_SIZE * 4)
// UART3 TX
#define  UART3_DMA_TX_BUF_SIZE            64
#define  UART3_TX_BUF_SIZE                UART3_DMA_TX_BUF_SIZE * 2

/* UART SECTION  END */


/* STM32F103C8 SECTION END */

#endif

