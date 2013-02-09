#ifndef LEDS_H_
#define LEDS_H_

#include "main.h"


#define LED_AUX_OK (1<<5)
#define LED_GPS_OK (1<<11)
#define LED_RC_OK (1<<1)
#define LED_MODE (1<<3)
#define LED_MAG_CAL (1<<14)
#define LED_GYRO_CAL (1<<13)
#define LED_STRICK_CAL (1<<0)
#define LED_SETUP (1<<15)
#define LED_N (1<<2)
#define LED_NE (1<<10)
#define LED_W (1<<9)
#define LED_SE (1<<12)
#define LED_S (1<<8)
#define LED_SW (1<<7)
#define LED_E (1<<4)
#define LED_NW (1<<6)

void INIT_Leds(void);
void led_event(void);

void LED_on(uint16_t led);
void LED_off(uint16_t led);
void LED_toggle(uint16_t led);
void LED_slowBlink(uint16_t led);
void LED_fastBlink(uint16_t led);

#endif
