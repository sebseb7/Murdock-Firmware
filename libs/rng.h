#ifndef _RNG_H
#define _RNG_H

#include "main.h"

typedef struct 
{
	__I uint32_t UID1; 
	__I uint32_t UID2;
	__I uint32_t UID3;
	__I uint32_t FLASH_SIZE;
} SIGNATURE_TypeDef;

#define SIGNATURE_BASE           ((uint32_t )0x1FFF7A10)
#define SIGNATURE              ((SIGNATURE_TypeDef *) SIGNATURE_BASE)

uint32_t RNG_Get(void);
void RNG_Enable(void);
void RNG_Disable(void);

#endif
