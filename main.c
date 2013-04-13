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
#include "libs/log.h"
#include "libs/sbus.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *	datasheet: http://www.st.com/web/en/resource/technical/document/datasheet/DM00037051.pdf
 *	user manual: http://www.st.com/st-web-ui/static/active/en/resource/technical/document/reference_manual/DM00031020.pdf
 */

static uint16_t buttonsInitialized = 0;


static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
static __IO uint32_t count_event = 0;
static __IO uint32_t rx1_event = 0;
static __IO uint32_t rx2_event = 0;
static __IO uint32_t sbus_event = 0;
static uint32_t sbus_failsafe = 0;

void ch1_rx_complete(void)
{
	rx1_event = 1;
}
void ch2_rx_complete(void)
{
	rx2_event = 1;
}
void sbus_rx_complete(void)
{
	sbus_event = 1;
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

uint32_t get_systick(void)
{
	return tick;
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

	//spektrum_init();
	sbus_init();
	
	pwm_init();

	//MPU & PMA
	i2c2_init();

	delay(120);

	BMP085_getCalData(); 
	MPU6050_Initialize();

	
	log_init();

	log_printf("boot\n");

#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif

	uint32_t led_counter = 0;
	uint32_t serial_output_counter = 0;
	uint32_t bind_counter = 4000;
	uint32_t button_counter = 0;
	uint32_t receiver_ok = 30000;
	uint32_t receiver_off = 1;
	



	float ch1=0.0f;
	float ch2=0.0f;
	float ch3=0.0f;
	float ch4=0.0f;
	float ch5=0.0f;
	float ch6=0.0f;
	float ch7=0.0f;


	while(1)  // main loop
	{
/*		if(rx1_event == 1)
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
				ch2 = (((rx[2]&3)<<8)+rx[3]-511) / 358.0f;
				ch6 = (((rx[4]&3)<<8)+rx[5]-511) / 358.0f;
				ch3 = (((rx[6]&3)<<8)+rx[7]-511) / 358.0f;
				ch4 = (((rx[8]&3)<<8)+rx[9]-511) / 358.0f;
				ch1 = (((rx[10]&3)<<8)+rx[11]-511) / 358.0f;
				ch5 = (((rx[12]&3)<<8)+rx[13]-511) / 358.0f;
				ch7 = (((rx[14]&3)<<8)+rx[15]-511) / 358.0f;
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
				ch2 = (((rx[2]&3)<<8)+rx[3]-511) / 358.0f;
				ch6 = (((rx[4]&3)<<8)+rx[5]-511) / 358.0f;
				ch3 = (((rx[6]&3)<<8)+rx[7]-511) / 358.0f;
				ch4 = (((rx[8]&3)<<8)+rx[9]-511) / 358.0f;
				ch1 = (((rx[10]&3)<<8)+rx[11]-511) / 358.0f;
				ch5 = (((rx[12]&3)<<8)+rx[13]-511) / 358.0f;
				ch7 = (((rx[14]&3)<<8)+rx[15]-511) / 358.0f;

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
		*/
		if(sbus_event == 1)
		{
			sbus_event = 0;
			uint8_t * rx = get_rx_sbus_buffer();
			
			if( 
				(rx[0] != 15)||
				(rx[24] != 0)||
				((rx[23]&0xf0) != 0)
			)
			{
#ifdef USE_USB_OTG_FS
				usb_printf("sbus out of sync\n");
#endif
				log_printf("sbus out of sync\n");
				delay(5);
				sbus_dma_init();
			}
			else
			{

				if( (rx[23]&0x8) == 0x8 )
				{
					if(sbus_failsafe==0)
					{
						log_printf("sbus failsafe\n");
					}
					sbus_failsafe=1;
					receiver_ok=0;
				}
				else
				{
					sbus_failsafe=0;
					receiver_ok=0;
					led_on(LED_RC_OK|LED_SBUS);//so this only once!
				}

				//parse sbus

				unsigned int current_bit_in_byte = 0;
				unsigned int current_bit_in_ch = 0;
				unsigned int current_ch = 0;
				unsigned int current_byte = 1;

				signed int channels[12];

				for(unsigned int i=0;i<12;i++)
				{
					channels[i] = 0;
				}

				for(unsigned int i=0;i<132;i++)
				{
					if(rx[current_byte] & (1<<current_bit_in_byte))
					{
						channels[current_ch] |= (1<<current_bit_in_ch);
					}

					current_bit_in_byte++;
					current_bit_in_ch++;

					if(current_bit_in_byte == 8)
					{
						current_bit_in_byte =0;
						current_byte++;
					}
					if(current_bit_in_ch == 11)
					{
						current_bit_in_ch =0;
						current_ch++;
					}
				}
#ifdef USE_USB_OTG_FS
				//usb_printf("SBUS: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",rx[0],rx[1],rx[2],rx[3],rx[4],rx[5],rx[6],rx[7],rx[8],rx[9],rx[10],rx[11],rx[12],rx[13],rx[14],rx[15],rx[16],rx[17],rx[18],rx[19],rx[20],rx[21],rx[22],rx[23],rx[24]);
				//usb_printf("SBUS: %u %u %u %u %u %u %u %u %u %u %u %u \n",channels[0],channels[1],channels[2],channels[3],channels[4],channels[5],channels[6],channels[7],channels[8],channels[9],channels[10],channels[11]);
#endif

				ch2 = (channels[0]-1024)/-672.0f;
				ch3 = (channels[1]-1024)/-672.0f;
				ch1 = (channels[2]-1024)/-672.0f;
				ch4 = (channels[3]-1024)/-672.0f;
				ch5 = (channels[4]-1024)/-672.0f;
				ch6 = (channels[5]-1024)/-672.0f;
				ch7 = (channels[6]-1024)/-672.0f;
				set_servo(3,ch1);
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch4);
				set_servo(6,ch5);
				set_servo(5,ch6);
				set_servo(7,ch7);

			}

		}


		if(count_event == 1)
		{
			count_event = 0;
			
			receiver_ok++;
	
			// original receiver diables PWM after two seconds
			if(receiver_ok > 20000)
			{
				if(receiver_off != 1)
				{
					led_fastBlink(LED_RC_OK);
					led_off(LED_SBUS);
					TIM_Cmd(TIM3, DISABLE);
					TIM_Cmd(TIM4, DISABLE);
					log_printf("receiver lost\n");
					receiver_off = 1;
				}
			}else if(sbus_failsafe == 1)
			{
				if(receiver_off != 2)
				{
					led_slowBlink(LED_RC_OK);
					led_slowBlink(LED_SBUS);
					TIM_Cmd(TIM3, DISABLE);
					TIM_Cmd(TIM4, DISABLE);
					receiver_off = 2;
				}

			}else if(receiver_off != 0)
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
	
//#ifdef USE_USB_OTG_FS
//				uint8_t * rx1 = get_rx1_buffer();
//				uint8_t * rx2 = get_rx2_buffer();
//				//usb_printf("1: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx1[0],rx1[1],rx1[2],rx1[3],rx1[4],rx1[5],rx1[6],rx1[7],rx1[8],rx1[9],rx1[10],rx1[11],rx1[12],rx1[13],rx1[14],rx1[15]);
//				//usb_printf("2: %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n", rx2[0],rx2[1],rx2[2],rx2[3],rx2[4],rx2[5],rx2[6],rx2[7],rx2[8],rx2[9],rx2[10],rx2[11],rx2[12],rx2[13],rx2[14],rx2[15]);
//				BMP085_readTemperature();
//				uint16_t temp = BMP085_getTemperatrue();
//				//usb_printf("temp: %u\n", temp);
//				
//				int16_t raw[6] = {0,0,0,0,0,0};
//				uint8_t FT[4] = {0,0,0,0};
//				while(MPU6050_GetFIFOCount()==0){delay(1);};
//
//				MPU6050_GetRawAccelGyro(raw);
//				//MPU6050_GetFT(FT);
//				uint8_t test = MPU6050_TestConnection();
//				uint8_t fifo = MPU6050_GetFIFOCount();
//				//usb_printf("raw: %u %u %i %i %i %i %i %i\n", test,fifo,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5]);
//				//log_printf("raw: %u %u %i %i %i %i %i %i\n", test,fifo,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5]);
//				MPU6050_ResetFIFOCount();
//#endif
			}

#ifdef USE_USB_OTG_FS
			serial_output_counter++;

			if(serial_output_counter > 220)
			{
				serial_output_counter=0;
				if(receiver_off == 0)
				{
					usb_printf("thr:%.3f ail:%.3f elev:%.3f rudd:%.3f gear:%.3f flap:%.3f\n",ch1,ch2,ch3,ch4,ch5,ch6);
				}
			}
#endif


		}
		if(buttons_get_press( KEY_B ))
		{
			if( buttons_get_state(KEY_A | KEY_C) == (KEY_A|KEY_C) )
			{
				led_fastBlink(LED_SP1|LED_SP2|LED_BIND);
				bind_counter=0;
			}
		}
	}
}

