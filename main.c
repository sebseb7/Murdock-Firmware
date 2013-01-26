#include "main.h"

//#include "lib/usb_serial.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *  data sheet : http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/DM00037051.pdf
 *
 *
 */

static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
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
	tick++;
}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIOB->ODR           |=       1<<13;
		GPIOB->ODR           |=       1<<12;

		// LEDs
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;       
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;       
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
	}

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* TIM config */

	GPIO_InitTypeDef GPIO_InitStructure;


	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);

	/* pwm set up */


	/* Compute the prescaler value */
	uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1;


	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 665;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;


	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;


	TIM_OC1Init(TIM4, &TIM_OCInitStructure);


	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);


	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;


	TIM_OC2Init(TIM4, &TIM_OCInitStructure);


	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);


	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;


	TIM_OC3Init(TIM4, &TIM_OCInitStructure);


	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);


	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;


	TIM_OC4Init(TIM4, &TIM_OCInitStructure);


	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);


	TIM_ARRPreloadConfig(TIM4, ENABLE);


	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);

	/* DEMO */

	int brightness = 0;


	while(1)  // Do not exit
	{
		brightness++;


		TIM4->CCR3 = 333 - (brightness + 0) % 333; // set brightness
		TIM4->CCR4 = 333 - (brightness + 166/2) % 333; // set brightness
		TIM4->CCR1 = 333 - (brightness + 333/2) % 333; // set brightness
		TIM4->CCR2 = 333 - (brightness + 499/2) % 333; // set brightness

		GPIOB->ODR           &=       ~(1<<12);
		Delay(100);
		GPIOB->ODR           |=       1<<12;
		Delay(100);
	}
}

