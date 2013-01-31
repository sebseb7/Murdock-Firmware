#include "main.h"

#include "libs/armmath.h"
#include "libs/usb_serial.h"
#include "libs/buttons.h"
#include "libs/leds.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 */

static uint16_t buttonsInitialized = 0;


static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
void Delay100us(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);
}
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime*10;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	static uint8_t button_sample = 0;

	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	tick++;
	if(buttonsInitialized)
	{
		button_sample++;
		if(button_sample < 100)
			return;
		button_sample = 0;

		sample_buttons();

		if(get_key_press( KEY_A ))
		{
			LED_toggle(1);
		}
	}
}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);


	INIT_Leds();
	INIT_Buttons();
	buttonsInitialized=1;

	//usb_serial_init();



	while(1)  // Do not exit
	{
		LED_toggle(0);

		Delay(1000);
	}
}

