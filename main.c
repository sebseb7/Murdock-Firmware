#include <stdlib.h>

#include "main.h"


//#include "libs/armmath.h"
#include "libs/usb_serial.h"
#include "libs/buttons.h"
#include "libs/leds.h"
#include "libs/spektrum.h"
#include "libs/rng.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *	datasheet: http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/DM00037051.pdf
 *	user manual: http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/REFERENCE_MANUAL/DM00031020.pdf
 */

static uint16_t buttonsInitialized = 0;


static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
static __IO uint32_t count_event = 0;

// systick counters for main loop


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
	count_event = 1;
	tick++;

	
	if(buttonsInitialized)
	{
		button_sample++;
		if(button_sample == 100)
		{
			button_sample = 0;
			sample_buttons();
		}
	}

}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);
	
	//use hardware RNG to initialize rand()
	RNG_Enable();
	srand(RNG_Get());
	RNG_Disable();


	INIT_Leds();
	INIT_Buttons();
	buttonsInitialized=1;

	Spektrum_init();
	

#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif

	uint16_t led_counter = 0;
	uint32_t bind_counter = 4000;


	while(1)  // main loop
	{
		if(count_event == 1)
		{
			count_event = 0;


			if (bind_counter < 3000)
			{ 
				Spektrum_bind(bind_counter);
				bind_counter++;
			}


			led_counter++;
			if(led_counter > 3000)
			{
				led_counter=0;
				LED_toggle(0);
			}

		}




		
		if(get_key_press( KEY_A ))
		{
			LED_on(1);
			bind_counter=0;
		}
	}
}

