
#include "buttons.h"

static uint16_t key_state;
static uint16_t key_press;


void buttons_sample(void)
{
	static uint16_t ct0, ct1;
	
	uint16_t key_curr =	
				((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)^1)<<2)|
				((GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)^1)<<1)|
				(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)^1);

	uint16_t i = key_state ^ ~key_curr;
	
	ct0 = ~( ct0 & i );
	ct1 = ct0 ^ (ct1 & i);
	i &= ct0 & ct1;
	key_state ^= i;
	key_press |= key_state & i;
}
	
//C6 as button input
void buttons_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_14 |GPIO_Pin_15;       
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
}


uint16_t buttons_get_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}

uint16_t buttons_get_state( uint16_t key_mask )
{
	return key_mask & key_state;
}

