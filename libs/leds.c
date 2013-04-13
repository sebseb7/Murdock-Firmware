
#include "leds.h"

//
// LEDs on B12 / B13
//
//




static uint16_t leds_on = 0;
static uint16_t leds_slowBlink = 0;
static uint16_t leds_fastBlink = 0;
static uint16_t leds_current_state = 0;

static void update_registers(void)
{
	for(unsigned int i = 0 ; i < 16;i++)
	{
		if(leds_current_state & (1<<i))
		{
			GPIOA->ODR           &=       ~(1<<2);
		}
		else
		{
			GPIOA->ODR           |=       (1<<2);
		}
		GPIOC->ODR           |=       (1<<5);
		GPIOC->ODR           &=       ~(1<<5);
		
	}
	GPIOA->ODR           |=       (1<<3);
	GPIOA->ODR           &=       ~(1<<3);
}

void led_event(void)
{
	static uint16_t blink_counter = 0;

	blink_counter++;

	if(blink_counter>7)
	{
		blink_counter=0;
	}

	int update  = 0;

	for(unsigned int i = 0;i < 16;i++)
	{
		if( (leds_on & (1<<i) )&&(~(leds_current_state & (1<<i)) ) )
		{
			update = 1;
			leds_current_state |= (1<<i);
		}
		else if( (~(leds_on & (1<<i)) )&&(leds_current_state & (1<<i) ) )
		{
			update = 1;
			leds_current_state &= ~(1<<i);
		}
		
		if( (leds_slowBlink & (1<<i) )&&(blink_counter == 0))
		{
			update = 1;
			leds_current_state |= (1<<i);
		}
		if( (leds_slowBlink & (1<<i) )&&(blink_counter == 4))
		{
			update = 1;
			leds_current_state &= ~(1<<i);
		}
		
		if( (leds_fastBlink & (1<<i) )&&((blink_counter&1) == 0))
		{
			update = 1;
			leds_current_state |= (1<<i);
		}
		if( (leds_fastBlink & (1<<i) )&&((blink_counter&1) == 1))
		{
			update = 1;
			leds_current_state &= ~(1<<i);
		}
	}
	if(update == 1)
	{
		update_registers();
	}
}


void led_on(uint16_t led)
{
	for(uint16_t i = 0;i<16;i++)
	{
		if(led & (1<<i))
		{
			leds_current_state |= (1<<i);
			leds_on |= (1<<i);
			leds_fastBlink &= ~(1<<i);
			leds_slowBlink &= ~(1<<i);
		}
	}
	update_registers();
}
void led_off(uint16_t led)
{
	for(uint16_t i = 0;i<16;i++)
	{
		if(led & (1<<i))
		{
			leds_current_state &= ~(1<<i);
			leds_on &= ~(1<<i);
			leds_fastBlink &= ~(1<<i);
			leds_slowBlink &= ~(1<<i);
		}
	}
	update_registers();
}
void led_toggle(uint16_t led)
{
	for(uint16_t i = 0;i<16;i++)
	{
		if(led & (1<<i))
		{
			leds_current_state ^= (1<<i);
			leds_on ^= (1<<i);
			leds_fastBlink &= ~(1<<i);
			leds_slowBlink &= ~(1<<i);
		}
	}
	update_registers();
}
void led_slowBlink(uint16_t led)
{
	for(uint16_t i = 0;i<16;i++)
	{
		if(led & (1<<i))
		{
			leds_slowBlink |= (1<<i);
			leds_fastBlink &= ~(1<<i);
			leds_on &= ~(1<<i);
		}
	}
}
void led_fastBlink(uint16_t led)
{
	for(uint16_t i = 0;i<16;i++)
	{
		if(led & (1<<i))
		{
			leds_fastBlink |= (1<<i);
			leds_slowBlink &= ~(1<<i);
			leds_on &= ~(1<<i);
		}
	}
}

void led_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIOA->ODR           |=       (1<<2);
	GPIOA->ODR           &=       ~(1<<3);
	GPIOC->ODR           &=       ~(1<<5);

	// LEDs
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;       
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;       
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	//A3 == latch
	//a2 == DATA
	//C5 == clock
	

	update_registers();


}

