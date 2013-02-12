#include "rng.h"
#include "main.h"

uint32_t rng_get(void)
{

	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);
	return RNG_GetRandomNumber();
}

void rng_enable(void)
{  
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}

void rng_disable(void)
{  
	RNG_Cmd(DISABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, DISABLE);
}

