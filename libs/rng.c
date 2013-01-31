#include "rng.h"
#include "main.h"

uint32_t RNG_Get(void)
{

	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);
	return RNG_GetRandomNumber();
}

void RNG_Enable(void)
{  
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
}

void RNG_Disable(void)
{  
	RNG_Cmd(DISABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, DISABLE);
}

