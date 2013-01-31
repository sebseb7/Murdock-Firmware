
#include "spektrum.h"
#include "uart.h"

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

void Spektrum_bind(void)
{

	//disable DMA A1
	//TODO
	//
	A1_GPIO();
	
	//powerdown
	GPIOA->ODR           |=       (1<<0);
	GPIOA->ODR           &=       ~(1<<1);

	Delay(2000);



	GPIOA->ODR           |=       (1<<1);
	//powerup
	GPIOA->ODR           &=       ~(1<<0);

	Delay(19);

	for(int i = 0;i < 5;i++)
	{
		GPIOA->ODR           &=       ~(1<<1);
		Delay100us(1);
		GPIOA->ODR           |=       (1<<1);
		Delay100us(1);
	}

	//disable GPIO A1 -> enable A1 UART RX
	//TODO

	//reenable DMA A1
	//TODO
}


