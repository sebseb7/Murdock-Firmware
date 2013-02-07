
#include "leds.h"

//
// LEDs on B12 / B13
//
//


#define LED_AUX_OK (1<<5)
#define LED_GPS_OK (1<<2)
#define LED_RC_OK (1<<1)
#define LED_MODE (1<<3)
#define LED_MAG_CAL (1<<14)
#define LED_GYRO_CAL (1<<15)
#define LED_STRICK_CAL (1<<0)
#define LED_SETUP (1<<13)
#define LED_N (1<<11)
#define LED_NE (1<<10)
#define LED_W (1<<9)
#define LED_SE (1<<12)
#define LED_S (1<<8)
#define LED_SW (1<<7)
#define LED_E (1<<4)
#define LED_NW (1<<6)


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
		//todo: update shift registers
	}

}


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

