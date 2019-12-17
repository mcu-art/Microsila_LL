#ifndef MI_LL_SETTINGS_H
#define MI_LL_SETTINGS_H
#include <microsila_ll/device/mi_device_list.h>
#include <microsila_ll/core/data_types.h>

// 16..64-bit SIZETYPES can be defined
// 32-bit SIZETYPE seems to be the most efficient for STM32
#define SIZETYPE uint32_t

// NOTE: BUFFER SIZES MUST BE DIVISIBLE BY 4 IN ORDER TO ALIGN DATA PROPERLY;
// NOT COMPLYING TO THIS RULE MAY LEAD TO BUGS THAT ARE DIFFICULT TO FIND

/* SOFTWARE MODULES BEGIN */
// Uncomment to use debug console to print text messages
// via uart on MCU or stdio on PC

#define USE_DBG_CONSOLE

// Define UART to be used by debug console;
// This has no effect on PC

#define DBG_CONSOLE_UART uart1

/* SOFTWARE MODULES END */


/* DEVICE SECTION  BEGIN */

#define MI_DEVICE STM32F103C8
#define MI_DEVICE_FAMILY F103

/* UART SECTION BEGIN */

/* Comment out UARTs that your project does not use */
//#define UART1_USE_RX
#define UART1_USE_TX
//#define UART2_USE_RX
//#define UART2_USE_TX
//#define UART3_USE_RX
//#define UART3_USE_TX



/* UART rx buffer size recommendations */
// 64 bytes is sufficient at rates <= 115200 baud
// 256 bytes is recommended at rates > 1Mbaud

// Too small size will increase CPU usage and may result in data loss

// Following definitions are not necessary to comment out,
// they do not have impact on memory usage if the uart is disabled

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

/* NON-VOLATILE STORAGE SECTION BEGIN */

// #define USE_NON_VOLATILE_STORAGE

/* NON-VOLATILE STORAGE SECTION END */


/* DEVICE SECTION END */

#endif

