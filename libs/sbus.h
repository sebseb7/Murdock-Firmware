#ifndef SBUS_H_
#define SBUS_H_

#include "main.h"

void sbus_init(void);
void sbus_dma_init(void);
uint8_t * get_rx_sbus_buffer(void);

#endif
