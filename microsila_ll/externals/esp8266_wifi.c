
#include "esp8266_wifi.h"




EspWifiStatus espWifiStatus = ESPWIFI_NOT_INIT;

typedef struct {
	const char* SSID;
	const char* passwd;
	const char* host;
	void (*on_data_rx_callback) (const uint8_t, ByteBuf*);
	uint16_t port;
	BOOL is_AP;
	uint32_t last_heartbeat_ts;
} EspWifi;

EspWifi wifi1;

void esp8266_init(void (*on_data_rx_callback) (const uint8_t channel_id, ByteBuf* data)) {
	wifi1.on_data_rx_callback = on_data_rx_callback;
	
}


void esp8266_enable(void) {
	
}


	
void esp8266_disable(void) {
	
}



inline EspWifiStatus esp8266_status(void) {
	return espWifiStatus;
}



void esp8266_start_AP(const char* name, const char* passwd) {
	
}



void esp8266_stop_AP(void) {
	
}



void esp8266_connect_to_AP(const char* name, const char* passwd) {
	
}



void esp8266_disconnect_from_AP(void) {
	
}



void esp8266_connect_to_host(const char* host, const uint16_t port) {
	
}



void esp8266_disconnect_from_host(void) {
	
}



inline BOOL esp8266_can_tx(void) {
	
}



OP_RESULT esp8266_tx(const uint8_t channel_id, ByteBuf* data) {
	
}



OP_RESULT esp8266_tx_arr(const uint8_t channel_id, const BYTE* data, const SIZETYPE size) {
	
}



// Callback function to be called when data from associated uart received
void esp8266_on_uart_rx(ByteBuf* data) {
	
}


void esp8266_main_process(uint64_t sys_millis) {
	
	
}


