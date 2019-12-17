#include "mi_ll_settings.h"
#ifdef USE_DBG_CONSOLE

#include "dbg_console.h"
#include <stdarg.h>
#include "utils.h"

#if MI_DEVICE == PC
#include <stdio.h>
#else
#include "../periph/uart1.h"
#include "../periph/uart2.h"
#include "../periph/uart3.h"
#endif

#if MI_DEVICE != PC
#ifndef DBG_CONSOLE_UART
#error "Because you defined `USE_DBG_CONSOLE`, please also define `DBG_CONSOLE_UART` in 'mi_ll_settings.h'!"
#endif
#endif

static BYTE dc_internal_buf[DC_INTERNAL_BUF_SIZE + sizeof (ByteBuf)];
static ByteBuf* dc_buf = 0;


#define DC_MACRO_UART_TX_FUNC_ENDING _tx_buf_all
#define DC_MACRO_UART_TX_ARR_FUNC_ENDING _tx_all



static inline void _tx_from_dc_buf_and_reset(void) {
    #if MI_DEVICE == PC
        if (!bb_free_space(dc_buf)) { bb_reset(dc_buf); return; }
        bb_write_byte_if_fits(dc_buf, 0); // write zero to terminate sting
        printf("%s", dc_buf->data);
    #else
        PPCAT(DBG_CONSOLE_UART, DC_MACRO_UART_TX_FUNC_ENDING)(dc_buf);
    #endif
        bb_reset(dc_buf);
}


void _dc_print(const char* str) {
    if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }
    bb_append_str(dc_buf, str);
    _tx_from_dc_buf_and_reset();
}


// Print formatted string
void _dc_printf(const char* fmt_str, ...) {
    if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }
    extern OP_RESULT mi_vfmt_str(ByteBuf* output, const char* s, va_list args);
    va_list arglist;
    va_start(arglist, fmt_str);
    mi_vfmt_str(dc_buf, fmt_str, arglist);
    _tx_from_dc_buf_and_reset();
    va_end(arglist);
}


void _dc_printbuf(ByteBuf* buf) {
    if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }
    SIZETYPE size = bb_unread_size(buf);
    if (size > DC_INTERNAL_BUF_SIZE - 1) {
      size  = DC_INTERNAL_BUF_SIZE - 1;
    }
    if (!size) { return; }
    bb_append_array_if_fits(dc_buf, buf->data, size);
    _tx_from_dc_buf_and_reset();
}

void _dc_printarr(const BYTE* arr, const SIZETYPE size) {
	if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }
	PPCAT(DBG_CONSOLE_UART, DC_MACRO_UART_TX_ARR_FUNC_ENDING)(arr, size);
}


void _dc_tracebuf(ByteBuf* buf) {
    if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }

    BOOL is_ok = ( (buf->rIndex <= buf->size) && (buf->rIndex <= buf->wIndex)
                   && (buf->wIndex <= buf->size));

    static const char* OK_MSG = "OK";
    static const char* INDEX_ERROR_MSG = "INDEX_ERROR";
    static const char* MSG_TEMPLATE = "* ByteBuf(size = %d): %s\n  * read: %d, write: %d\n  * Data: '";

    mi_fmt_str(dc_buf, MSG_TEMPLATE, buf->size,
               (is_ok) ? OK_MSG : INDEX_ERROR_MSG, buf->rIndex, buf->wIndex);
    _tx_from_dc_buf_and_reset();
    SIZETYPE size = bb_unread_size(buf);
    if (size > DC_INTERNAL_BUF_SIZE - 1) {
      size  = DC_INTERNAL_BUF_SIZE - 1;
    }
    bb_append_array_if_fits(dc_buf, buf->data, size);
    bb_append_str(dc_buf, "'\n");
    _tx_from_dc_buf_and_reset();
}


/*
void _dc_tracebuf(ByteBuf* buf) {
    if (!dc_buf) { dc_buf = bb_init(dc_internal_buf, DC_INTERNAL_BUF_SIZE); }

    //bb_reset(dc_buf);
    BOOL is_ok = ( (buf->rIndex <= buf->size) && (buf->rIndex <= buf->wIndex)
                   && (buf->wIndex <= buf->size));

    mi_fmt_str(dc_buf, "* ByteBuf(size = %d): ", (uint32_t) buf->size);
    _tx_from_dc_buf_and_reset();

    const char * msg;
    if (is_ok) {
        msg = "OK\n";
    } else {
       msg = "INDEX_ERROR\n";
    }
    bb_append_str(dc_buf, msg);
    _tx_from_dc_buf_and_reset();

    mi_fmt_str(dc_buf, "  * read: %d, write: %d\n", buf->rIndex, buf->wIndex);
    _tx_from_dc_buf_and_reset();

    bb_append_str(dc_buf, "  * Data: '");

    SIZETYPE size = bb_unread_size(buf);
    if (size > DC_INTERNAL_BUF_SIZE - 1) {
      size  = DC_INTERNAL_BUF_SIZE - 1;
    }
    bb_append_array_if_fits(dc_buf, buf->data, size);
    bb_append_str(dc_buf, "'\n");
    _tx_from_dc_buf_and_reset();
}
*/

#endif // USE_DBG_CONSOLE
