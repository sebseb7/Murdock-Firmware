#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"

void TimingDelay_Decrement(void);
void delay(__IO uint32_t nTime);
uint32_t get_systick(void);
void enter_system_bootloader(void);
void ch1_rx_complete(void);
void ch2_rx_complete(void);
void sbus_rx_complete(void);
void i2c_error(uint8_t);


#endif
