
#include "spektrum.h"
/*
 *
 * spektrum 18.4ms after powerup
 *
 * 4 low pulses 120µs , 120µs spacing
 *
 *
 *
 */

void Spektrum_init(void)
{
	//A0 as output
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;       
	GPIOA->ODR           |=       (1<<0);
	GPIO_Init(GPIOA, &GPIO_InitStructure);  


	//dma usart init
	//TODO
	

	//powerup
	GPIOA->ODR           &=       ~(1<<0);

}

void Spektrum_bind(void)
{
	//powerdown
	GPIOA->ODR           |=       (1<<0);

	Delay(300);

	//disable DMA A0
	//TODO

	//enable GPIO A0 out
	//TODO


	GPIOA->ODR           |=       (1<<1);
	//powerp
	GPIOA->ODR           &=       ~(1<<0);

	Delay(19);

	for(int i = 0;i < 5;i++)
	{
		GPIOA->ODR           &=       ~(1<<1);
		Delay100us(1);
		GPIOA->ODR           |=       (1<<1);
		Delay100us(1);
	}

	//disable GPIO A0 -> enable A0 UART RX
	//TODO

	//reenable DMA
	//TODO
}


