#ifndef UART_H_
#define UART_H_

#include "main.h"

void uart_init(void);
uint8_t * get_rx1_buffer(void);
uint8_t * get_rx2_buffer(void);
void uart_dma_init(void);

#endif
