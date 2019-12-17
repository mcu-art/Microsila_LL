
/* Debug console; print debug messages via UART on MCUs or stdio on PC */


#ifndef DBG_CONSOLE_H
#define DBG_CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_DBG_CONSOLE

#include <mi_ll_settings.h>
#include "data_types.h"
#include "byte_buf.h"

// Size of the debug consol internal buffer
#define DC_INTERNAL_BUF_SIZE  256

// Print zero-terminated string
extern void _dc_print(const char* str);

// Print formatted string (light-weight, fast but limited version of std::printf)
extern void _dc_printf(const char* fmt_str, ...);

// Print unread contents of the buffer
extern void _dc_printbuf(ByteBuf* buf);

// Print array of specified size
extern void _dc_printarr(const BYTE* arr, const SIZETYPE size);

// Print information about the byte buffer
// and trace its unread contents (up to 1024 bytes)
extern void _dc_tracebuf(ByteBuf* buf);

// Macros
#define dc_print(strzero) \
    do { _dc_print(strzero); } while(0)

#define dc_printf(format, ...) \
    do { _dc_printf(format, ##__VA_ARGS__); } while(0)

#define dc_printbuf(buf) do { _dc_printbuf(buf); } while(0)

#define dc_printarr(arr, size) do { _dc_printarr(arr, size); } while(0)

#define dc_tracebuf(buf) do { _dc_tracebuf(buf); } while(0)

#define dc_trace(var) do { _dc_printf("'%s' = %d \n", #var, var); } while(0)

#define dc_tracef(var) do { _dc_printf("'%s' = %5f \n", #var, var); } while(0)

#else // USE_DBG_CONSOLE

#define dc_print(strzero) do { } while(0)
#define dc_printf(format, ...) do { } while(0)
#define dc_printbuf(buf) do { } while(0)
#define dc_printarr(arr, size) do { } while(0)
#define dc_tracebuf(buf) do { } while(0)
#define dc_trace(var) do { } while(0)
#define dc_tracef(var) do { } while(0)

#endif // USE_DBG_CONSOLE

#ifdef __cplusplus
}
#endif

#endif // DBG_CONSOLE_H
