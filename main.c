#include <stdlib.h>

#include "main.h"


#include "libs/armmath.h"
#include "libs/usb_serial.h"
#include "libs/buttons.h"
#include "libs/leds.h"
#include "libs/spektrum.h"
#include "libs/uart.h"
#include "libs/rng.h"
#include "libs/i2c.h"
#include "libs/bmp085.h"
#include "libs/mpu.h"
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

void ch1_rx_complete(void)
{
	rx1_event = 1;
}
void ch2_rx_complete(void)
{
	rx2_event = 1;
}


void delay(__IO uint32_t nTime)
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


	led_init();
	buttons_init();
	buttonsInitialized=1;

	spektrum_init();
	
	pwm_init();

	//MPU & PMA
	i2c2_init();

	delay(120);

	BMP085_getCalData(); 
	MPU6050_Initialize();

#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif
	
/*	while(1)
	{
		for(int i = 0;i<128;i++)
		{
			int retval = i2c_start(I2C2, i, I2C_Direction_Transmitter); 
			i2c_stop(I2C2);
			usb_printf("%u %u\n",i,retval);
			delay(100);
		}	
		delay(3000);
	}
*/	

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
			uint8_t * rx = get_rx1_buffer();


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
				usb_printf("sp1 out of sync\n");
#endif
				delay(5);
				uart_dma_init();
			}
			else
			{
				float ch2 = ((((rx[2]&3)<<8)+rx[3]) / 512.0f)-1;
				float ch6 = ((((rx[4]&3)<<8)+rx[5]) / 512.0f)-1;
				float ch3 = ((((rx[6]&3)<<8)+rx[7]) / 512.0f)-1;
				float ch4 = ((((rx[8]&3)<<8)+rx[9]) / 512.0f)-1;
				float ch1 = ((((rx[10]&3)<<8)+rx[11]) / 512.0f)-1;
				float ch5 = ((((rx[12]&3)<<8)+rx[13]) / 512.0f)-1;
				float ch7 = ((((rx[14]&3)<<8)+rx[15]) / 512.0f)-1;
				set_servo(3,ch1);
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch4);
				set_servo(6,ch5);
				set_servo(5,ch6);
				set_servo(7,ch7);
				receiver_ok=0;
				led_on(LED_RC_OK);
			}
		}
		if(rx2_event == 1)
		{
			rx2_event = 0;
			uint8_t * rx = get_rx2_buffer();


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
				usb_printf("sp2 out of sync\n");
#endif
				delay(5);
				uart_dma_init();
			}
			else
			{
				float ch2 = ((((rx[2]&3)<<8)+rx[3]) / 512.0f)-1;
				float ch6 = ((((rx[4]&3)<<8)+rx[5]) / 512.0f)-1;
				float ch3 = ((((rx[6]&3)<<8)+rx[7]) / 512.0f)-1;
				float ch4 = ((((rx[8]&3)<<8)+rx[9]) / 512.0f)-1;
				float ch1 = ((((rx[10]&3)<<8)+rx[11]) / 512.0f)-1;
				float ch5 = ((((rx[12]&3)<<8)+rx[13]) / 512.0f)-1;
				float ch7 = ((((rx[14]&3)<<8)+rx[15]) / 512.0f)-1;
				set_servo(3,ch1);
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch4);
				set_servo(6,ch5);
				set_servo(5,ch6);
				set_servo(7,ch7);
				receiver_ok=0;
				led_on(LED_RC_OK);
			}
		}
		if(count_event == 1)
		{
			count_event = 0;
			
			receiver_ok++;
	
			// original receiver diables PWM after two seconds
			if(receiver_ok > 20000)
			{
				led_fastBlink(LED_RC_OK);
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
					buttons_sample();
				}
			}

			if (bind_counter < 4000)
			{ 
				spektrum_bind(bind_counter);
				bind_counter++;
			}

			led_counter++;

			if(led_counter > 1000)
			{	
				led_event();
				led_counter=0;
	
#ifdef USE_USB_OTG_FS
				uint8_t * rx1 = get_rx1_buffer();
				uint8_t * rx2 = get_rx2_buffer();
				usb_printf("1: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx1[0],rx1[1],rx1[2],rx1[3],rx1[4],rx1[5],rx1[6],rx1[7],rx1[8],rx1[9],rx1[10],rx1[11],rx1[12],rx1[13],rx1[14],rx1[15]);
				usb_printf("2: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx2[0],rx2[1],rx2[2],rx2[3],rx2[4],rx2[5],rx2[6],rx2[7],rx2[8],rx2[9],rx2[10],rx2[11],rx2[12],rx2[13],rx2[14],rx2[15]);
				BMP085_readTemperature();
				uint16_t temp = BMP085_getTemperatrue();
				usb_printf("temp: %u\n", temp);
				
				int16_t raw[6] = {0,0,0,0,0,0};
				uint8_t FT[4] = {0,0,0,0};
				while(MPU6050_GetFIFOCount()==0){delay(1);};

				MPU6050_GetRawAccelGyro(raw);
				//MPU6050_GetFT(FT);
				uint8_t test = MPU6050_TestConnection();
				uint8_t fifo = MPU6050_GetFIFOCount();
				usb_printf("raw: %u %u %i %i %i %i %i %i\n", test,fifo,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5]);
				MPU6050_ResetFIFOCount();
#endif
			}

		}
		if(buttons_get_press( KEY_B ))
		{
			if( buttons_get_state(KEY_A | KEY_C) == (KEY_A|KEY_C) )
			{
				led_off(LED_CIRCLE);
				led_fastBlink(LED_SP1|LED_SP2|LED_BIND);
				bind_counter=0;
			}
		}
	}
}

