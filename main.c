#include <stdlib.h>

#include "main.h"


#include "libs/armmath.h"
#include "libs/usb_serial.h"
#include "libs/buttons.h"
#include "libs/leds.h"
#include "libs/spektrum.h"
#include "libs/uart.h"
#include "libs/rng.h"
#include "libs/pwm.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *	datasheet: http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/DM00037051.pdf
 *	user manual: http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/REFERENCE_MANUAL/DM00031020.pdf
 */

static uint16_t buttonsInitialized = 0;


static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
static __IO uint32_t count_event = 0;
static __IO uint32_t rx1_event = 0;
static __IO uint32_t rx2_event = 0;

void Ch1_rx_complete(void)
{
	rx1_event = 1;
}
void Ch2_rx_complete(void)
{
	rx2_event = 1;
}

float ch1=0.0f;
float ch2=0.0f;
float ch3=0.0f;
float ch4=0.0f;
float ch5=0.0f;
float ch6=0.0f;
float ch7=0.0f;


void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime*10;
	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	count_event = 1;
	tick++;
}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);
	
	//use hardware RNG to initialize rand()
//	RNG_Enable();
//	srand(RNG_Get());
//	RNG_Disable();


	INIT_Leds();
	INIT_Buttons();
	buttonsInitialized=1;

	Spektrum_init();
	
	PWM_Init();
	

#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif

	uint32_t led_counter = 0;
	uint32_t bind_counter = 4000;
	uint32_t button_counter = 0;
	uint32_t receiver_ok = 0;
	uint32_t receiver_off = 1;


	while(1)  // main loop
	{
		if(rx1_event == 1)
		{
			rx1_event = 0;
			uint8_t * rx = getRx1Buffer();


			if( 
				(rx[1] != 0x01)||
				((rx[2]&0xFC) != 0x04)||
				((rx[4]&0xFC) != 0x14)||
				((rx[6]&0xFC) != 0x08)||
				((rx[8]&0xFC) != 0x0C)||
				((rx[10]&0xFC) != 0x00)||
				((rx[12]&0xFC) != 0x10)||
				((rx[14]&0xFC) != 0x18)
			)
			{
#ifdef USE_USB_OTG_FS
				usbprintf("sp1 out of sync\n");
#endif
				Delay(5);
				USART_DMA_Init();
			}
			else
			{
				ch2 = ((((rx[2]&3)<<8)+rx[3]) / 512.0f)-1;
				ch6 = ((((rx[4]&3)<<8)+rx[5]) / 512.0f)-1;
				ch3 = ((((rx[6]&3)<<8)+rx[7]) / 512.0f)-1;
				ch4 = ((((rx[8]&3)<<8)+rx[9]) / 512.0f)-1;
				ch1 = ((((rx[10]&3)<<8)+rx[11]) / 512.0f)-1;
				ch5 = ((((rx[12]&3)<<8)+rx[13]) / 512.0f)-1;
				ch7 = ((((rx[14]&3)<<8)+rx[15]) / 512.0f)-1;
				set_servo(3,ch1);
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch4);
				set_servo(6,ch5);
				set_servo(5,ch6);
				set_servo(7,ch7);
				receiver_ok=0;
				LED_on(LED_RC_OK);
			}
		}
		if(rx2_event == 1)
		{
			rx2_event = 0;
			uint8_t * rx = getRx2Buffer();


			if( 
				(rx[1] != 0x01)||
				((rx[2]&0xFC) != 0x04)||
				((rx[4]&0xFC) != 0x14)||
				((rx[6]&0xFC) != 0x08)||
				((rx[8]&0xFC) != 0x0C)||
				((rx[10]&0xFC) != 0x00)||
				((rx[12]&0xFC) != 0x10)||
				((rx[14]&0xFC) != 0x18)
			)
			{
#ifdef USE_USB_OTG_FS
				usbprintf("sp2 out of sync\n");
#endif
				Delay(5);
				USART_DMA_Init();
			}
			else
			{
				ch2 = ((((rx[2]&3)<<8)+rx[3]) / 512.0f)-1;
				ch6 = ((((rx[4]&3)<<8)+rx[5]) / 512.0f)-1;
				ch3 = ((((rx[6]&3)<<8)+rx[7]) / 512.0f)-1;
				ch4 = ((((rx[8]&3)<<8)+rx[9]) / 512.0f)-1;
				ch1 = ((((rx[10]&3)<<8)+rx[11]) / 512.0f)-1;
				ch5 = ((((rx[12]&3)<<8)+rx[13]) / 512.0f)-1;
				ch7 = ((((rx[14]&3)<<8)+rx[15]) / 512.0f)-1;
				set_servo(3,ch1);
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch4);
				set_servo(6,ch5);
				set_servo(5,ch6);
				set_servo(7,ch7);
				receiver_ok=0;
				LED_on(LED_RC_OK);
			}
		}
		if(count_event == 1)
		{
			count_event = 0;
			
			receiver_ok++;
	
			// original receiver diables PWM after two seconds
			if(receiver_ok > 20000)
			{
				LED_fastBlink(LED_RC_OK);
				TIM_Cmd(TIM3, DISABLE);
				TIM_Cmd(TIM4, DISABLE);
				receiver_off = 1;

			}else if(receiver_off == 1)
			{
				receiver_off = 0;
				TIM_Cmd(TIM3, ENABLE);
				TIM_Cmd(TIM4, ENABLE);
			}
	
			if(buttonsInitialized)
			{
				button_counter++;
				if(button_counter == 30)
				{
					button_counter = 0;
					sample_buttons();
				}
			}

			if (bind_counter < 4000)
			{ 
				Spektrum_bind(bind_counter);
				bind_counter++;
			}

			led_counter++;

			if(led_counter > 1000)
			{	
				led_event();
				led_counter=0;
	
#ifdef USE_USB_OTG_FS
				uint8_t * rx1 = getRx1Buffer();
				uint8_t * rx2 = getRx2Buffer();
				usbprintf("1: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx1[0],rx1[1],rx1[2],rx1[3],rx1[4],rx1[5],rx1[6],rx1[7],rx1[8],rx1[9],rx1[10],rx1[11],rx1[12],rx1[13],rx1[14],rx1[15]);
				usbprintf("2: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx2[0],rx2[1],rx2[2],rx2[3],rx2[4],rx2[5],rx2[6],rx2[7],rx2[8],rx2[9],rx2[10],rx2[11],rx2[12],rx2[13],rx2[14],rx2[15]);
#endif
			}

		}
		if(get_key_press( KEY_B ))
		{
			if( get_key_state(KEY_A | KEY_C) == (KEY_A|KEY_C) )
			{
				LED_off(LED_CIRCLE);
				LED_fastBlink(LED_SP1|LED_SP2|LED_BIND);
				bind_counter=0;
			}
		}
	}
}

