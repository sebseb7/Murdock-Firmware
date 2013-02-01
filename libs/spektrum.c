
#include "spektrum.h"
#include "uart.h"
#include "leds.h"

/*
 *
 * spektrum 18.4ms after powerup
 *
 * 4 low pulses 120µs , 120µs spacing
 *
 *
 *
 */
	
static void A1_GPIO(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;       
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

static void A1_UART(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;       
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
}

void Spektrum_init(void)
{
	//A0 as output
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;       
	GPIOA->ODR           |=       (1<<0);
	GPIO_Init(GPIOA, &GPIO_InitStructure);  


	//enable A1 UART RX
	//dma usart init
	
	A1_UART();
	UART_Init();

	//powerup
	GPIOA->ODR           &=       ~(1<<0);

}

void Spektrum_bind(uint32_t counter)
{
	if(counter == 0)
	{

		//disable DMA A1
		//TODO
		//
		A1_GPIO();
	
		//powerdown
		GPIOA->ODR           |=       (1<<0);
		GPIOA->ODR           &=       ~(1<<1);
		return;
	}

	if(counter == 2000)
	{
		GPIOA->ODR           |=       (1<<1);
		//powerup
		GPIOA->ODR           &=       ~(1<<0);
		return;
	}
	
	if(counter == 2900)
	{
		GPIOA->ODR           &=       ~(1<<1);
		return;
	}
	if(counter == 2901)
	{
		GPIOA->ODR           |=       (1<<1);
		return;
	}
	if(counter == 2902)
	{
		GPIOA->ODR           &=       ~(1<<1);
		return;
	}
	if(counter == 2903)
	{
		GPIOA->ODR           |=       (1<<1);
		return;
	}
	if(counter == 2904)
	{
		GPIOA->ODR           &=       ~(1<<1);
		return;
	}
	if(counter == 2905)
	{
		GPIOA->ODR           |=       (1<<1);
		return;
	}
	if(counter == 2906)
	{
		GPIOA->ODR           &=       ~(1<<1);
		return;
	}
	if(counter == 2907)
	{
		GPIOA->ODR           |=       (1<<1);
		return;
	}
		
	if(counter == 2910)
	{
		//disable GPIO A1 -> enable A1 UART RX
		A1_UART();

		//reenable DMA A1
		//TODO
	
		LED_off(1);
	}

}


