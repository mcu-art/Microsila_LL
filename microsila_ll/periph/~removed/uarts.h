
/* Common device-independent definitions and API for UARTs. */


#ifndef MI_UARTS_H
#define MI_UARTS_H

#include <mi_ll_settings.h>

#ifndef MI_DEVICE
#error Microsila_LL: MI_DEVICE not defined in file <mi_ll_settings.h>
#endif

#if (MI_DEVICE == STM32F103C8)
#include "../device/f103/uart1.h"
#include "../device/f103/uart2.h"
#include "../device/f103/uart3.h"
#else
#error Microsila_LL: unsupported MI_DEVICE defined in file <mi_ll_settings.h>
#endif

#endif
