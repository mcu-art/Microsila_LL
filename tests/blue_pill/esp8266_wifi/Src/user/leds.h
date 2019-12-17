#ifndef LEDS_H
#define LEDS_H

#include <mi_ll_settings.h>

#define TOGGLING_SPEED_FAST    2000
#define TOGGLING_SPEED_NORMAL  20000

// Must be called on every timer interrupt
extern inline void leds_on_timer_isr(void);

extern inline void InfoLedOn(void);
extern inline void InfoLedOff(void);
extern inline void InfoLedToggle(void);


extern inline void led_signal_success(void);
extern inline void led_signal_timeout(void);
extern inline void led_signal_error(void);
extern inline void led_signal_disable(void);

#endif
