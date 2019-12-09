
/* Common device-independent definitions and API for UARTs. */


#ifndef _UARTS_H
#define _UARTS_H

#include <mi_ll_settings.h>

#ifndef MI_DEVICE
#error Microsila_LL: MI_DEVICE not defined in file <mi_ll_settings.h>
#endif

#if (MI_DEVICE == STM32F103C)
#include "../device/f103/stm32f103_uart1.h"
#include "../device/f103/stm32f103_uart2.h"
#include "../device/f103/stm32f103_uart3.h"
#else
#error Microsila_LL: unsupported MI_DEVICE defined in file <mi_ll_settings.h>
#endif

#endif
