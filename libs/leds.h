#ifndef LEDS_H_
#define LEDS_H_

#include "main.h"


#define LED_AUX_OK (1<<10)
#define LED_GPS_OK (1<<13)
#define LED_RC_OK (1<<14)
#define LED_MODE (1<<12)
#define LED_MAG_CAL (1<<1)
#define LED_GYRO_CAL (1<<0)
#define LED_STICK_CAL (1<<15)
#define LED_SETUP (1<<2)
#define LED_N (1<<4)
#define LED_NE (1<<5)
#define LED_W (1<<8)
#define LED_SE (1<<3)
#define LED_S (1<<7)
#define LED_SW (1<<11)
#define LED_E (1<<6)
#define LED_NW (1<<9)


#define LED_SBUS LED_NW
#define LED_PPM LED_N
#define LED_BIND LED_NE
#define LED_SP1 LED_W
#define LED_SP2 LED_E
#define LED_SDCARD LED_SW
#define LED_SENSOR LED_S
#define LED_USB LED_SE

#define LED_CIRCLE LED_N|LED_NW|LED_W|LED_SW|LED_S|LED_SE|LED_E|LED_NE
#define LED_OK LED_AUX_OK|LED_GPS_OK|LED_RC_OK
#define LED_CAL LED_MAG_CAL|LED_GYRO_CAL|LED_STICK_CAL
#define LED_ALL LED_CIRCLE|LED_OK|LED_CAL|LED_MODE|LED_SETUP

void led_init(void);
void led_event(void);

void led_on(uint16_t led);
void led_off(uint16_t led);
void led_toggle(uint16_t led);
void led_slowBlink(uint16_t led);
void led_fastBlink(uint16_t led);

#endif
