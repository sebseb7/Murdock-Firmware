#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);
void Delay100us(__IO uint32_t nTime);

void Ch1_rx_complete(void);
void Ch2_rx_complete(void);



#endif
