#ifndef UART_H_
#define UART_H_

#include "main.h"

void UART_Init(void);
uint8_t * getRx1Buffer(void);
uint8_t * getRx2Buffer(void);
void USART_DMA_Init(void);

#endif
