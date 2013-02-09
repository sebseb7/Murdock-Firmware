#ifndef LEDS_H_
#define LEDS_H_

#include "main.h"

void INIT_Leds(void);

void LED_on(uint8_t led);
void LED_off(uint8_t led);
void LED_toggle(uint8_t led);
void LED_slowBlink(uint8_t led);
void LED_fastBlink(uint8_t led);

#endif
