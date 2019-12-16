
#include <mi_ll_settings.h>


/* Device-independent */

#include "microsila_ll/core/byte_buf.c"
#include "microsila_ll/core/utils.c"
#include "microsila_ll/core/crc.c"
#include "microsila_ll/core/dbg_console.c"
/* Device-specific */


#ifndef MI_DEVICE
#error Microsila_LL: MI_DEVICE must be defined in file <mi_ll_settings.h>
#endif


#if MI_DEVICE == STM32F103C

// UART1
#if defined(UART1_USE_RX) || defined(UART1_USE_TX)
#include "microsila_ll/device/f103/stm32f103_uart1.c"
#endif


// UART2
#if defined(UART2_USE_RX) || defined(UART2_USE_TX)
#include "microsila_ll/device/f103/stm32f103_uart2.c"
#endif

// UART3
#if defined(UART13_USE_RX) || defined(UART3_USE_TX)
#include "microsila_ll/device/f103/stm32f103_uart3.c"
#endif

#else  // unsupported device
#error Microsila_LL: unsupported MI_DEVICE defined in file <mi_ll_settings.h>
#endif
