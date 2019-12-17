
/* Common device-independent definitions and API for UARTs. */


#ifndef NON_VOLATILE_STORAGE_H
#define NON_VOLATILE_STORAGE_H

#include <mi_ll_settings.h>

#ifndef MI_DEVICE
#error Microsila_LL: MI_DEVICE not defined in file <mi_ll_settings.h>
#endif

#if (MI_DEVICE == STM32F103C)
#include "../device/f103/non_volatile_storage.h"
#else
#error Microsila_LL: unsupported MI_DEVICE defined in file <mi_ll_settings.h>
#endif

#endif
