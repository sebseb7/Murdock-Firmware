
#include "leds.h"

//
// LEDs on B12 / B13
//
//

void LED_on(uint8_t led)
{
	if(led == 0)
	{
		GPIOA->ODR           &=       ~(1<<2);
	}
	if(led == 1)
	{
		GPIOA->ODR           &=       ~(1<<2);
	}
}
void LED_off(uint8_t led)
{
	if(led == 0)
	{
		GPIOA->ODR           |=       1<<2;
	}
	if(led == 1)
	{
		GPIOA->ODR           |=       1<<2;
	}
}
void LED_toggle(uint8_t led)
{
	if(led == 0)
	{
		GPIOA->ODR           ^=       1<<2;
	}
	if(led == 1)
	{
		GPIOA->ODR           ^=       1<<2;
	}
}

void INIT_Leds(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIOA->ODR           |=       1<<2;
//	GPIOB->ODR           |=       1<<12;

	// LEDs
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;       
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;       
//	GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

