
/*  This file includes source files according to current mi_ll_settings.h */
#include <mi_ll_settings.h>


/* DEVICE-INDEPENDENT SECTION BEGIN */

#include "microsila_ll/core/byte_buf.c"
#include "microsila_ll/core/utils.c"
#include "microsila_ll/core/crc.c"
#include "microsila_ll/core/dbg_console.c"

#ifdef USE_TEST_HELPERS
#include "microsila_ll/core/test_helpers.c"
#endif

/* DEVICE-INDEPENDENT SECTION END */


/* DEVICE-UNIQUE SECTION BEGIN */
#if(MI_DEVICE != PC)  // peripherals are included only for MCUs

#ifndef MI_DEVICE
#error Microsila_LL: Please, define MI_DEVICE in  <mi_ll_settings.h>, see <mi_device_list.h>
#endif

#ifndef MI_DEVICE_FAMILY
#error Microsila_LL: Please, define MI_DEVICE_FAMILY in <mi_ll_settings.h>, see <mi_device_list.h>
#endif

#define STRINGIFY_MACRO(x) STR(x)
#define STR(x) #x
#define EXPAND(x) x
#define JOIN_PATH(n1, n2, n3) STRINGIFY_MACRO(EXPAND(n1)EXPAND(n2)EXPAND(n3))

// Directory where device-dependent code resides
#define DEVICE_DIR_BASE microsila_ll/device/


// UART1
#if defined(UART1_USE_RX) || defined(UART1_USE_TX)
#include JOIN_PATH(DEVICE_DIR_BASE, MI_DEVICE_FAMILY, uart1.c)
#endif


// UART2
#if defined(UART2_USE_RX) || defined(UART2_USE_TX)
#include JOIN_PATH(DEVICE_DIR_BASE, MI_DEVICE_FAMILY, uart2.c)
#endif

// UART3
#if defined(UART13_USE_RX) || defined(UART3_USE_TX)
#include JOIN_PATH(DEVICE_DIR_BASE, MI_DEVICE_FAMILY, uart3.c)
#endif

#if defined(USE_NON_VOLATILE_STORAGE)
#include JOIN_PATH(DEVICE_DIR_BASE, MI_DEVICE_FAMILY, non_volatile_storage.c)
#endif


#endif // (MI_DEVICE != PC)
/* DEVICE-UNIQUE SECTION END */

/* EXTERNAL MODULES SECTION BEGIN */

#ifdef USE_ESP8266_WIFI
#include "microsila_ll/externals/esp8266_wifi.c"
#endif

/* EXTERNAL MODULES SECTION END */
