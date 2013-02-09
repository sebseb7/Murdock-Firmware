
#include "leds.h"

//
// LEDs on B12 / B13
//
//




static uint16_t led_on = 0;
static uint16_t led_slowBlink = 0;
static uint16_t led_fastBlink = 0;
static uint16_t led_current_state = 0;


void led_event(void)
{
	static uint16_t blink_counter = 0;

	blink_counter++;

	if(blink_counter>8)
	{
		blink_counter=0;
	}

	int update  = 0;

	for(int i = 0;i < 16;i++)
	{
		if( (led_on & (1<<i) )&&(~(led_current_state & (1<<i)) ) )
		{
			update = 1;
			led_current_state |= (1<<i);
		}
		else if( (~(led_on & (1<<i)) )&&(led_current_state & (1<<i) ) )
		{
			update = 1;
			led_current_state &= ~(1<<i);
		}
		
		if( (led_slowBlink & (1<<i) )&&(blink_counter == 0))
		{
			update = 1;
			led_current_state |= (1<<i);
		}
		if( (led_slowBlink & (1<<i) )&&(blink_counter == 4))
		{
			update = 1;
			led_current_state &= ~(1<<i);
		}
		
		if( (led_fastBlink & (1<<i) )&&((blink_counter&1) == 0))
		{
			update = 1;
			led_current_state |= (1<<i);
		}
		if( (led_fastBlink & (1<<i) )&&((blink_counter&1) == 1))
		{
			update = 1;
			led_current_state &= ~(1<<i);
		}
	}
	if(update == 1)
	{
		for(int i = 0 ; i < 16;i++)
		{
			if(led_current_state & (1<<i))
			{
				GPIOA->ODR           |=       (1<<2);
			}
			else
			{
				GPIOA->ODR           &=       ~(1<<2);
			}
			__NOP();
			__NOP();
			__NOP();
		
		
			GPIOC->ODR           |=       (1<<5);
			__NOP();
			__NOP();
			__NOP();
			GPIOC->ODR           &=       ~(1<<5);
			__NOP();
			__NOP();
			__NOP();
			
		}
		GPIOA->ODR           |=       (1<<3);
			__NOP();
			__NOP();
		__NOP();
		GPIOA->ODR           &=       ~(1<<3);
			__NOP();
			__NOP();
		__NOP();
	
	}

}


void LED_on(uint16_t led)
{
	led_on |= 1<<led;
	led_fastBlink &= ~(1<<led);
	led_slowBlink &= ~(1<<led);
}
void LED_off(uint16_t led)
{
	led_on &= ~(1<<led);
	led_fastBlink &= ~(1<<led);
	led_slowBlink &= ~(1<<led);
}
void LED_toggle(uint16_t led)
{
	led_on ^= (1<<led);
	led_fastBlink &= ~(1<<led);
	led_slowBlink &= ~(1<<led);
}
void LED_slowBlink(uint16_t led)
{
	led_slowBlink |= (1<<led);
	led_fastBlink &= ~(1<<led);
	led_on &= ~(1<<led);
}
void LED_fastBlink(uint16_t led)
{
	led_fastBlink |= (1<<led);
	led_slowBlink &= ~(1<<led);
	led_on &= ~(1<<led);
}

void INIT_Leds(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIOA->ODR           &=       ~(1<<2);
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
	

	for(int i = 0 ; i < 16;i++)
	{
		GPIOC->ODR           |=       (1<<5);
		__NOP();
		__NOP();
		__NOP();
		GPIOC->ODR           &=       ~(1<<5);
		__NOP();
		__NOP();
		__NOP();
		
	}
	GPIOA->ODR           |=       (1<<3);
		__NOP();
		__NOP();
	__NOP();
	GPIOA->ODR           &=       ~(1<<3);
		__NOP();
		__NOP();
	__NOP();


}

