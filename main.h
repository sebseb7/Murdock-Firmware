#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"


#define MODE_NORMAL 1
#define MODE_CORR_HORIZ 2
#define MODE_CORR_NOSE 3
#define MODE_BIAS_CAL 4

#define AP_MODE_FREE 1
#define AP_MODE_STAB 2
#define AP_MODE_HOME 3

void TimingDelay_Decrement(void);
void delay(__IO uint32_t nTime);
uint32_t get_systick(void);
void enter_system_bootloader(void);
void ch1_rx_complete(void);
void ch2_rx_complete(void);
void sbus_rx_complete(void);
void i2c_error(uint8_t);
void event_loop(uint8_t sd_available);



#endif
