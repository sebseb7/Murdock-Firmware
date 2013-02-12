
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
	
static void spektrum_gpio(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_11;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void spektrum_uart(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_11;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
}

void spektrum_init(void)
{
	//B120 as output
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIOB->ODR           |=       (1<<12);
	GPIO_Init(GPIOB, &GPIO_InitStructure);  


	//enable C7 C11 UART RX
	//dma usart init
	
	spektrum_uart();
	uart_init();

	//powerup
	GPIOB->ODR           &=       ~(1<<12);

}

void spektrum_bind(uint32_t counter)
{
	if(counter == 0)
	{

		//disable DMA C7 C11
		//TODO
		//
		spektrum_gpio();
	
		//powerdown
		GPIOB->ODR           |=       (1<<12);
		GPIOC->ODR           &=       ~(1<<7);
		GPIOC->ODR           &=       ~(1<<11);
		return;
	}

	if(counter == 2000)
	{
		GPIOC->ODR           |=       (1<<7);
		GPIOC->ODR           |=       (1<<11);
		//powerup
		GPIOB->ODR           &=       ~(1<<12);
		return;
	}
	
	if(counter == 2900)
	{
		GPIOC->ODR           &=       ~(1<<7);
		GPIOC->ODR           &=       ~(1<<11);
		return;
	}
	if(counter == 2901)
	{
		GPIOC->ODR           |=       (1<<7);
		GPIOC->ODR           |=       (1<<11);
		return;
	}
	if(counter == 2902)
	{
		GPIOC->ODR           &=       ~(1<<7);
		GPIOC->ODR           &=       ~(1<<11);
		return;
	}
	if(counter == 2903)
	{
		GPIOC->ODR           |=       (1<<7);
		GPIOC->ODR           |=       (1<<11);
		return;
	}
	if(counter == 2904)
	{
		GPIOC->ODR           &=       ~(1<<7);
		GPIOC->ODR           &=       ~(1<<11);
		return;
	}
	if(counter == 2905)
	{
		GPIOC->ODR           |=       (1<<7);
		GPIOC->ODR           |=       (1<<11);
		return;
	}
	if(counter == 2906)
	{
		GPIOC->ODR           &=       ~(1<<7);
		GPIOC->ODR           &=       ~(1<<11);
		return;
	}
	if(counter == 2907)
	{
		GPIOC->ODR           |=       (1<<7);
		GPIOC->ODR           |=       (1<<11);
		return;
	}
		
	if(counter == 3999)
	{
		//disable GPIO C7 C11 -> enable  UART RX
		spektrum_uart();

		//reenable DMA C7 C11
		//TODO
	
		led_off(LED_SP1|LED_SP2|LED_BIND);
	}

}


