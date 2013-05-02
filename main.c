#include <stdlib.h>
#include <math.h>

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
#include "libs/imu.h"
#include "libs/sbus.h"
#include "libs/microsd_spi.h"
#include "libs/pid.h"
#include "libs/qoffset.h"
#include "libs/config.h"

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
static __IO uint32_t ms10_count = 0;
static uint32_t sbus_failsafe = 0;
static uint32_t i2c_errors = 0;
static uint32_t i2c_e[7] = {0,0,0,0,0,0,0};
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

void i2c_error(uint8_t error)
{
	i2c_e[error-1]++;
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
	ms10_count++;
	if(ms10_count == 100)
	{
		ms10_count=0;
		disk_timerproc();
	}
	count_event = 1;
	tick++;
}

uint32_t get_systick(void)
{
	return tick;
}

void enter_system_bootloader(void)
{
	void (*SysMemBootJump)(void) = (void (*)(void)) (*((uint32_t *) 0x1FFF0004));
	__set_PRIMASK(1);
	RCC_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
	//RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
//	__set_MSP(0x20001000);
	__set_MSP(*(volatile unsigned int *)0x1FFFF000);
	SysMemBootJump();
	while(1);
}
static	int16_t min_acc_x = 0;

static	uint32_t last_write =0;
static	uint32_t last_sync =0;
static	uint32_t led_counter = 0;
	//static uint32_t serial_output_counter = 0;
static	uint32_t mpu_poll_counter = 0;
static	uint32_t sd_sync_counter = 0;
static	uint32_t mpu_output_counter = 0;
static	uint32_t bind_counter = 4000;
static	uint32_t button_counter = 0;
static	uint32_t receiver_ok = 30000;
static	uint32_t receiver_off = 1;
static	uint32_t wwdg_counter=0;
static  uint32_t mode = MODE_NORMAL;
static  uint32_t autopilot_mode = AP_MODE_FREE;



static	float ch1=0.0f;
static	float ch2=0.0f;
static	float ch3=0.0f;
static	float ch4=0.0f;
static	float ch5=0.0f;
static	float ch6=0.0f;
static	float ch7=0.0f;

static struct quaternion_t qcorrQ;

struct vector_t measured1;
struct vector_t measured2;

static	int16_t bias_gyro_x=0;
static	int16_t bias_gyro_y=0;
static	int16_t bias_gyro_z=0;
static	int16_t bias_acc_x=0;
static	int16_t bias_acc_y=0;
static	int16_t bias_acc_z=0;

static uint32_t bias_cal_count=0;
	
static struct pid_conf_t roll_pid;
static struct pid_conf_t pitch_pid;
//struct pid_conf_t alt_pid;
//struct pid_conf_t heading_pid;

#define ACC_FILTER_SIZE 15
static float filter_acc_x_window[ACC_FILTER_SIZE];
static float filter_acc_y_window[ACC_FILTER_SIZE];
static float filter_acc_z_window[ACC_FILTER_SIZE];
static float filtered_acc_x =0.0f;;
static float filtered_acc_y =0.0f;;
static float filtered_acc_z =0.0f;;
static uint32_t filter_acc_index=0;

#define PID_FILTER_SIZE 5
static float filter_pid_roll_window[ACC_FILTER_SIZE];
static float filter_pid_nick_window[ACC_FILTER_SIZE];
//static float filtered_pid_roll =0.0f;;
//static float filtered_pid_nick =0.0f;;
//static uint32_t filter_pid_index=0;

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


	for(int i = 0;i<ACC_FILTER_SIZE;i++)
	{
		filter_acc_x_window[i]=0.0f;
		filter_acc_y_window[i]=0.0f;
		filter_acc_z_window[i]=0.0f;
	}

	for(int i = 0;i<PID_FILTER_SIZE;i++)
	{
		filter_pid_roll_window[i]=0.0f;
		filter_pid_nick_window[i]=0.0f;
	}

	led_init();
	buttons_init();
	buttonsInitialized=1;

	spektrum_init();
	sbus_init();
	
	pwm_init();

	//MPU & PMA
	i2c2_init();

	delay(120);

	//BMP085_getCalData(); 
	MPU6050_Initialize();

	
	log_init();

	log_printf("boot\n");

	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{ 
		log_printf("watchdog reset\n");
		RCC_ClearFlag();
	}

#ifdef USE_USB_OTG_FS
	usb_serial_init();
#endif


	led_fastBlink(LED_SETUP);

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	/* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)  */
//	WWDG_SetPrescaler(WWDG_Prescaler_8);
//	WWDG_SetWindowValue(127);
//	WWDG_Enable(126);

	pid_init(&roll_pid);
	pid_init(&pitch_pid);
	//pid_init(&alt_pid);
	//pid_init(&heading_pid);
	
	pitch_pid.Kp = 15.4f;
	pitch_pid.Ki = 0.0937f; //langsam 
	pitch_pid.Kd = 0.0202377f;

	roll_pid.Kp = 10.4f;
	roll_pid.Ki = 0.0937f; //langsam 
	roll_pid.Kd = 0.0202377f;

	load_config(&qcorrQ.w,&qcorrQ.x,&qcorrQ.y,&qcorrQ.z,&bias_gyro_x,&bias_gyro_y,&bias_gyro_z,&bias_acc_x,&bias_acc_y,&bias_acc_z);
			
	log_printf("load config: %f %f %f %f %i %i %i %i %i %i\n",qcorrQ.w,qcorrQ.x,qcorrQ.y,qcorrQ.z,bias_gyro_x,bias_gyro_y,bias_gyro_z,bias_acc_x,bias_acc_y,bias_acc_z);

	/*
	qcorrQ.w=1.0f;
	qcorrQ.x=0.0f;
	qcorrQ.y=0.0f;
	qcorrQ.z=0.0f;
	bias_gyro_x=0;
	bias_gyro_y=0;
	bias_gyro_z=0;
	bias_acc_x=0;
	bias_acc_y=0;
	bias_acc_z=0;
	*/
	
	bias_gyro_x=-38;
	bias_gyro_y=-10;
	bias_gyro_z=-35;
	bias_acc_x=180;
	bias_acc_y=-20;
	bias_acc_z=215;

	while(1)  // main loop
	{
		event_loop(1);
	}
}

void event_loop(uint8_t sd_available)
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
			if(sd_available) log_printf("sbus out of sync\n");

			WWDG_SetCounter(126);
			delay(3);
			WWDG_SetCounter(126);
			sbus_dma_init();
		}
		else
		{

			if( (rx[23]&0x8) == 0x8 )
			{
				if(sbus_failsafe==0)
				{
					if(sd_available) log_printf("sbus failsafe\n");
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

			ch2 = (channels[0]-1024)/672.0f;
			ch3 = (channels[1]-1024)/672.0f;
			ch1 = (channels[2]-1024)/672.0f;
			ch4 = (channels[3]-1024)/672.0f;
			ch5 = (channels[4]-1024)/672.0f;
			ch6 = (channels[5]-1024)/672.0f;
			ch7 = (channels[6]-1024)/672.0f;

			//Nuri aileron mix
			//float servo1 = ch3 *  0.6f - ch2;
			//float servo2 = ch3 * -0.6f - ch2;
			//set_servo(1,servo1);
			//set_servo(2,servo2);

			if(ch4 < 0.2f)
			{
				set_servo(1,ch2);
				set_servo(2,ch3);
				set_servo(4,ch2);
			}

			set_servo(3,ch1);

			//set_servo(4,ch4);
			//set_servo(6,ch5);
			//set_servo(5,ch6);
			//set_servo(7,ch7);

		}

	}


	if(count_event == 1)
	{
		count_event = 0;


		wwdg_counter++;
		if(wwdg_counter == 40)
		{
			wwdg_counter=0;;
		}

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
				if(sd_available) log_printf("receiver lost\n");
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

#ifdef USE_USB_OTG_FS
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
			//				//if(sd_available) log_printf("raw: %u %u %i %i %i %i %i %i\n", test,fifo,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5]);
			//				MPU6050_ResetFIFOCount();
#endif
		}

#ifdef USE_USB_OTG_FS
		//	serial_output_counter++;
		//
		//	if(serial_output_counter > 2200)
		//	{
		//i2c2_init();
		//		serial_output_counter=0;
		//		if(receiver_off == 0)
		//		{
		//			usb_printf("thr:%.3f ail:%.3f elev:%.3f rudd:%.3f gear:%.3f flap:%.3f\n",ch1,ch2,ch3,ch4,ch5,ch6);
		//		}
		//	}
#endif


		mpu_poll_counter++;

		if(mpu_poll_counter > 10)
		{
			mpu_poll_counter=0;

			unsigned int start_time = get_systick();
			unsigned int int_status = MPU6050_GetIntStatus();
			unsigned int diff = get_systick()-start_time;
			//usb_printf("diff: %i , %i (%i) (%i %i %i %i %i %i %i)\n",diff,int_status,i2c_errors,i2c_e[0],i2c_e[1],i2c_e[2],i2c_e[3],i2c_e[4],i2c_e[5],i2c_e[6]);


			if(diff > 10)
			{
				if(sd_available) log_printf("i2c error\n");
				i2c_errors++;
				I2C_Cmd(I2C2, DISABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
				WWDG_SetCounter(126);
				delay(1);
				WWDG_SetCounter(126);
				i2c2_init();
			} else if(int_status & 1)
			{
				// wenn seit ner gewissen zeit int_status nicht mehr 1 war, i2c resetten und mpu neu initialisieren

				int16_t raw[6] = {0,0,0,0,0,0};
				unsigned int start_time = get_systick();
				MPU6050_GetRawAccelGyro(raw);
				unsigned int diff = get_systick()-start_time;

				if(diff > 10)
				{
					if(sd_available) log_printf("i2c error\n");
					i2c_errors++;
					I2C_Cmd(I2C2, DISABLE);
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
					WWDG_SetCounter(126);
					delay(1);
					WWDG_SetCounter(126);
					i2c2_init();
				}
				else
				{
					//usb_printf("%6i %6i %6i %6i %6i %6i (%i,%i)  \n",raw[0],raw[1],raw[2],raw[3],raw[4],raw[5],diff,i2c_errors);
					//usb_printf("VALS: %i %f %f %f %f %f %f %f %i\n",mpu_cnt,q0,q1,q2,q3,yaw,pitch,roll,i2c_errors);

					//		gyro_x = 0;
					//		gyro_y = 0;
					//		gyro_z = 0;
					if( raw[0] < min_acc_x )
					{
						min_acc_x = raw[0];
					}

					float acc_x = (raw[0] - bias_acc_x) / 61436.25f;
					float acc_y = (raw[1] - bias_acc_y) / 61436.25f;
					float acc_z = (raw[2] - bias_acc_z) / 61436.25f;
					//float acc_x = (raw[0] - bias_acc_x) / 16383.0f;
					//float acc_y = (raw[1] - bias_acc_y) / 16383.0f;
					//float acc_z = (raw[2] - bias_acc_z) / 16383.0f;
					
					float gyro_x = (raw[3] - bias_gyro_x) / 938.7197f;
					float gyro_y = (raw[4] - bias_gyro_y) / 938.7197f;
					float gyro_z = (raw[5] - bias_gyro_z) / 938.7197f;
					
					//g forces: float sum = acc_x*acc_x + acc_y*acc_y + acc_z*acc_z;
			


					if(mode == MODE_CORR_HORIZ)
					{
						measured1.x=acc_x;
						measured1.y=acc_y;
						measured1.z=acc_z;
					}
					if(mode == MODE_CORR_NOSE)
					{
						measured2.x=acc_x;
						measured2.y=acc_y;
						measured2.z=acc_z;
					}
					if(mode == MODE_BIAS_CAL)
					{
						bias_cal_count++;

						if( abs(raw[0] - bias_acc_x) > 60)
						{
							//bias_acc_x = (raw[0]>>1) + (bias_acc_x>>1);
							bias_cal_count=0;
						}
						if( abs(raw[1] - bias_acc_y) > 60)
						{
							//bias_acc_y = (raw[1]>>1) + (bias_acc_y>>1);
							bias_cal_count=0;
						}
						if( abs((raw[2]-16383) - bias_acc_z) > 60)
						{
							//bias_acc_z = ((raw[2]-16383)>>1) +  (bias_acc_z>>1);
							bias_cal_count=0;
						}
					
						if( abs(raw[3] - bias_gyro_x) > 3)
						{
							//bias_gyro_x = raw[3];
							bias_cal_count=0;
						}
						if( abs(raw[4] - bias_gyro_y) > 3)
						{
							//bias_gyro_y = raw[4];
							bias_cal_count=0;
						}
						if( abs(raw[5] - bias_gyro_z) > 3)
						{
							//bias_gyro_z = raw[5];
							bias_cal_count=0;
						}
					
						//usb_printf(" %i %i %i %i %i %i %i\n",abs(raw[0] - bias_acc_x),abs(raw[1] - bias_acc_y),abs((raw[2]-16383) - bias_acc_z),abs(raw[3] - bias_gyro_x),abs(raw[4] - bias_gyro_y),abs(raw[5] - bias_gyro_x),bias_cal_count);
					
						if(bias_cal_count==1000)
						{
							mode = MODE_NORMAL;
							led_off(LED_GYRO_CAL);
							save_config(qcorrQ.w,qcorrQ.x,qcorrQ.y,qcorrQ.z,bias_gyro_x,bias_gyro_y,bias_gyro_z,bias_acc_x,bias_acc_y,bias_acc_z);
							log_printf("save config: %f %f %f %f %i %i %i %i %i %i\n",qcorrQ.w,qcorrQ.x,qcorrQ.y,qcorrQ.z,bias_gyro_x,bias_gyro_y,bias_gyro_z,bias_acc_x,bias_acc_y,bias_acc_z);
						}
					
					
					}

					//full scale (-/+32767.5) is 2000deg/sec
					//convert to rad/sec

					//float gyro_x = (raw[3] / 32767.5f) * 2000.0f * (M_PI/180.0f);
					//float gyro_y = (raw[4] / 32767.5f) * 2000.0f * (M_PI/180.0f);
					//float gyro_z = (raw[5] / 32767.5f) * 2000.0f * (M_PI/180.0f);
					// 2000 = 938.7197 ; 1000  = 1877.4395 ; 500 = 3754.8789 ; 250 = 7509.7578
					//

					//acc_x=0;
					//acc_y=0;
					//acc_z=0;
					//gyro_x+=0.057f;
					//gyro_y-=0.0027;
					//gyro_z-=0.023f;


					filtered_acc_x += acc_x;
					filtered_acc_x -= filter_acc_x_window[filter_acc_index];
					filter_acc_x_window[filter_acc_index]=acc_x;
					
					filtered_acc_y += acc_y;
					filtered_acc_y -= filter_acc_y_window[filter_acc_index];
					filter_acc_y_window[filter_acc_index]=acc_y;
					
					filtered_acc_z += acc_z;
					filtered_acc_z -= filter_acc_z_window[filter_acc_index];
					filter_acc_z_window[filter_acc_index]=acc_z;
					
					filter_acc_index++;
					if(filter_acc_index == ACC_FILTER_SIZE)
						filter_acc_index=0;
					

					acc_x = filtered_acc_x;
					acc_y = filtered_acc_y;
					acc_z = filtered_acc_z;

					MadgwickAHRSupdateIMU(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z);

					float yaw;
					float pitch;
					float roll;

					struct quaternion_t uncorrQ;
					uncorrQ.w = q0;
					uncorrQ.x = q1;
					uncorrQ.y = q2;
					uncorrQ.z = q3;
					struct quaternion_t correctedQ = quaternion_mul(&uncorrQ,&qcorrQ);

					GetEulerAngles(correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,&yaw,&pitch,&roll);


					float ap_roll = 0.0f;
					float ap_nick = 0.0f;

			
					if(ch4 >= 0.2f)
					{
						if(autopilot_mode == AP_MODE_FREE)
						{
							autopilot_mode = AP_MODE_STAB;
							led_slowBlink(LED_MODE);
		
							pid_reset(&pitch_pid);
							pid_reset(&roll_pid);
							
						}


						if(fabsf(roll) < ((float)M_PI/2.0f))
						{
							pitch*=-1.0f;
						}
						float pitch_deg = (pitch/(float)M_PI)*180.0f;
						float roll_deg = (roll/(float)M_PI)*180.0f;


						pitch_pid.Kp = 15.4f*(ch7+1)*(ch7+1);
						roll_pid.Kp = 10.4f*(ch7+1)*(ch7+1);
			
						float elev_out =  pid(&pitch_pid,ch5/10.0f,pitch_deg/90.0f, 0.005f);
						float ail_out =   pid(&roll_pid,ch6/10.0f,roll_deg/90.0f, 0.005f);
					
						/*
						ail_out /= (float)PID_FILTER_SIZE;
						filtered_pid_roll += ail_out;
						filtered_pid_roll -= filter_pid_roll_window[filter_pid_index];
						filter_pid_roll_window[filter_pid_index]= ail_out;
						
						elev_out /= (float)PID_FILTER_SIZE;
						filtered_pid_nick += elev_out;
						filtered_pid_nick -= filter_pid_nick_window[filter_pid_index];
						filter_pid_nick_window[filter_pid_index]= elev_out;
					
						filter_pid_index++;
						if(filter_pid_index == PID_FILTER_SIZE)
							filter_pid_index=0;
					

						ail_out  = filtered_pid_roll;
						elev_out = filtered_pid_nick;
						*/
						set_servo(1,ail_out*-1.0f);
						set_servo(4,ail_out*-1.0f);
						set_servo(2,elev_out);

						ap_roll = ail_out;
						ap_nick = elev_out;
					}
					else
					{
						if(autopilot_mode != AP_MODE_FREE)
						{
							autopilot_mode = AP_MODE_FREE;
							led_off(LED_MODE);
							
						}
					}

						
					mpu_output_counter++;

					if(mpu_output_counter > 7)
					{
						mpu_output_counter=0;
#ifdef USE_USB_OTG_FS
						//usb_printf(" %i %i %i %i %i %i %i\n",raw[0],raw[1],raw[2],raw[3],raw[4],raw[5],bias_cal_count);
						//usb_printf(" %i %2i %3i %i %f %i %i %i %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %i %i\n",sd_available,last_write,last_sync,min_acc_x,sum,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5],q0,q1,q2,q3,yaw,pitch,roll,ch1,ch2,ch3,ch4,ch5,ch6,ch7,diff,i2c_errors);
						
						
						//usb_printf(" %f %f %f\n",pitch_deg,roll_deg,elev_out);
						//usb_printf("%f %f %f %f %f %f %f %f %f %f\n",correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z);
						usb_printf("%f %f %f %f %i %i %i %i %i %i\n",correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,raw[0]- bias_acc_x,raw[1]- bias_acc_y,raw[2]- bias_acc_z,raw[3] - bias_gyro_x,raw[4] - bias_gyro_y,raw[5] - bias_gyro_z);
						//usb_printf("%f %f %f %f %f %f %f %f %f %f %f\n",q0,q1,q2,q3,correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,pitch,roll,yaw);
#endif
						unsigned int start_time = get_systick();
						if(sd_available) log_printf("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,acc_x,acc_y,acc_z,gyro_x,gyro_y,gyro_z,ch1,ch2,ch3,ch4,ch5,ch6,ch7,ap_roll,ap_nick);
						//usb_printf("%f %f %f %f %f %f %f %f %f %f %f\n",q0,q1,q2,q3,correctedQ.w,correctedQ.x,correctedQ.y,correctedQ.z,pitch,roll,yaw);
						//if(sd_available) log_printf(" %2i %3i %i %f %i %i %i %i %i %i %f %f %f %f %f %f %f %f %f %f %f %f %f %f %i %i\n",last_write,last_sync,min_acc_x,sum,raw[0],raw[1],raw[2],raw[3],raw[4],raw[5],q0,q1,q2,q3,yaw,pitch,roll,ch1,ch2,ch3,ch4,ch5,ch6,ch7,diff,i2c_errors);
						last_write = get_systick()-start_time;

						if(last_sync > 1000)
						{
							//while(1);
						}
						last_sync = 0;
					}

				}

			}


			if(sd_available)
			{
				sd_sync_counter++;
	
				if(sd_sync_counter > 1000)
				{
					sd_sync_counter=0;
					WWDG_SetCounter(126);
					unsigned int start_time = get_systick();
					log_sync();
					last_sync = get_systick()-start_time;
				}
			}
			WWDG_SetCounter(126);
		}

		if(buttons_get_press( KEY_B ))
		{
			if( buttons_get_state(KEY_A | KEY_C) == (KEY_A|KEY_C) )
			{
				led_fastBlink(LED_SP1|LED_SP2|LED_BIND);
				bind_counter=0;
			}
		}

		if(buttons_get_press( KEY_C ))
		{
			if(mode == MODE_NORMAL)
			{
				if( buttons_get_state(KEY_A | KEY_B) == (KEY_A|KEY_B) )
				{
					led_fastBlink(LED_GYRO_CAL);
					mode = MODE_CORR_HORIZ;
				}
			} 
			else if(mode == MODE_CORR_HORIZ)
			{
				led_slowBlink(LED_GYRO_CAL);
				mode = MODE_CORR_NOSE;
			}
			else if(mode == MODE_CORR_NOSE)
			{
				led_off(LED_GYRO_CAL);
				mode = MODE_NORMAL;
				
				struct vector_t expected1 = {0.0f,0.0f,1.0f};
				struct vector_t expected2 = {0.0f,1.0f,0.0f};
				struct quaternion_t tmp = get_corrQ(&expected1,&expected2,&measured1,&measured2);
				qcorrQ = quaternion_conj(&tmp);

				save_config(qcorrQ.w,qcorrQ.x,qcorrQ.y,qcorrQ.z,bias_gyro_x,bias_gyro_y,bias_gyro_z,bias_acc_x,bias_acc_y,bias_acc_z);
	
				log_printf("save config: %f %f %f %f %i %i %i %i %i %i\n",qcorrQ.w,qcorrQ.x,qcorrQ.y,qcorrQ.z,bias_gyro_x,bias_gyro_y,bias_gyro_z,bias_acc_x,bias_acc_y,bias_acc_z);

				//usb_printf("m1: %f %f %f\n",measured1.x,measured1.y,measured1.z);
				//usb_printf("m2: %f %f %f\n",measured2.x,measured2.y,measured2.z);
				//usb_printf("corr: %f %f %f %f\n",tmp.w,tmp.x,tmp.y,tmp.z);
			}
		}
		if(buttons_get_press( KEY_A ))
		{
			if(mode == MODE_NORMAL)
			{
				if( buttons_get_state(KEY_C | KEY_B) == (KEY_C|KEY_B) )
				{
					led_on(LED_GYRO_CAL);
					bias_cal_count=0;
					mode = MODE_BIAS_CAL;
				}
			} 
		}
	}
}

