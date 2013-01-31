
#include "leds.h"

//
// LEDs on B12 / B13
//
//

void LED_on(uint8_t led)
{
	if(led == 0)
	{
		GPIOB->ODR           &=       ~(1<<12);
	}
	if(led == 1)
	{
		GPIOB->ODR           &=       ~(1<<13);
	}
}
void LED_off(uint8_t led)
{
	if(led == 0)
	{
		GPIOB->ODR           |=       1<<12;
	}
	if(led == 1)
	{
		GPIOB->ODR           |=       1<<13;
	}
}
void LED_toggle(uint8_t led)
{
	if(led == 0)
	{
		GPIOB->ODR           ^=       1<<12;
	}
	if(led == 1)
	{
		GPIOB->ODR           ^=       1<<13;
	}
}

void INIT_Leds(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIOB->ODR           |=       1<<13;
	GPIOB->ODR           |=       1<<12;

	// LEDs
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;       
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;       
	GPIO_Init(GPIOB, &GPIO_InitStructure);  
}

