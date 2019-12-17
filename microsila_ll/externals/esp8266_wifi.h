
/* ESP8266 wifi module */


#ifndef MI_ESP8266_WIFI_H
#define MI_ESP8266_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mi_ll_settings.h>
#include "../core/byte_buf.h"
	
typedef enum {
	ESPWIFI_NOT_INIT = 0,
	ESPWIFI_IDLE,  // ok but there is no active tcp connection
	ESPWIFI_CONNECTED, // active connection esteblished
	ESPWIFI_BUSY,
	ESPWIFI_ERROR
} EspWifiStatus;


// DELETED
// void (*power_on_func) (BOOL on),
// `power_on_func` is a function that receives BOOL and powers on/off the module;

// Init the esp8266 module;
// `on_data_rx_callback`: a callback function that takes 
// connection id and data received by ESP8266
extern void esp8266_init(void (*on_data_rx_callback) 
	                       (const uint8_t channel_id, ByteBuf* data));

extern void esp8266_enable(void);
	
extern void esp8266_disable(void);

extern inline EspWifiStatus esp8266_status(void);

extern void esp8266_start_AP(const char* name, const char* passwd);

extern void esp8266_stop_AP(void);

extern void esp8266_connect_to_AP(const char* name, const char* passwd);

extern void esp8266_disconnect_from_AP(void);

extern void esp8266_connect_to_host(const char* host, const uint16_t port);

extern void esp8266_disconnect_from_host(void);

extern inline BOOL esp8266_can_tx(void);

extern OP_RESULT esp8266_tx(const uint8_t channel_id, ByteBuf* data);

extern OP_RESULT esp8266_tx_arr(const uint8_t channel_id, const BYTE* data, const SIZETYPE size);

// Callback function to be called when data from associated uart received
extern void esp8266_on_uart_rx(ByteBuf* data);

// To be called periodically from main()
extern void esp8266_main_process(uint64_t sys_millis);

#ifdef __cplusplus
}
#endif

#endif
