
#include "leds.h"
#include "main.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"

typedef struct {
	volatile uint32_t toggling_speed;
	volatile uint32_t count;
	volatile BOOL enabled;
} InfoLedIndicator;


InfoLedIndicator info_led_indicator;

inline void leds_on_timer_isr(void) {
	++info_led_indicator.count;
	if (info_led_indicator.count == info_led_indicator.toggling_speed) {
		info_led_indicator.count = 0;
		if (info_led_indicator.enabled) {
			InfoLedToggle();
		}
	}
}


inline void InfoLedToggle(void) {
	LL_GPIO_TogglePin(INFO_LED_GPIO_Port, INFO_LED_Pin);
}

inline void InfoLedOn(void) {
	LL_GPIO_ResetOutputPin(INFO_LED_GPIO_Port, INFO_LED_Pin);
}

inline void InfoLedOff(void) {
	LL_GPIO_SetOutputPin(INFO_LED_GPIO_Port, INFO_LED_Pin);
}

inline void InfoLedBlinkFast(void) {
	info_led_indicator.enabled = TRUE;
	info_led_indicator.toggling_speed = TOGGLING_SPEED_FAST;
}


inline void InfoLedBlinkNormal(void) {
	info_led_indicator.enabled = TRUE;
	info_led_indicator.toggling_speed = TOGGLING_SPEED_NORMAL;
	
}


inline void InfoLedBlinkDisable(void) {
	info_led_indicator.enabled = FALSE;
	info_led_indicator.toggling_speed = TOGGLING_SPEED_NORMAL;
	InfoLedOff();
}

inline void led_signal_success(void) {
	InfoLedBlinkNormal();
}

void led_signal_error(void) {
	info_led_indicator.enabled = FALSE;
	info_led_indicator.toggling_speed = TOGGLING_SPEED_NORMAL;
	InfoLedOn();
}

inline void led_signal_timeout(void) {
	InfoLedBlinkFast();
}


inline void led_signal_disable(void) {
	InfoLedBlinkDisable();
}

