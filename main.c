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

	Spektrum_init();


#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif
#ifdef USE_USB_OTG_FS
	RNG_Enable();
	usbprintf("%u %u %u %u\n",SIGNATURE->UID1,SIGNATURE->UID2,SIGNATURE->UID3,RNG_Get());
#endif

	while(1)  // Do not exit
	{
		LED_toggle(0);

		Delay(50);
		
		if(get_key_press( KEY_A ))
		{
			LED_toggle(1);
			Spektrum_bind();
		}
	}
}

