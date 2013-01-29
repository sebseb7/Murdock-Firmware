#include "main.h"

#include "libs/armmath.h"
#include "libs/usb_serial.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 */

/*
 *
 * spektrum 18.4ms after powerup
 *
 * 4 low pulses 120µs , 120µs spacing
 *
 *
 *
 */
static uint16_t key_state;
static uint16_t key_press;
static uint16_t buttonsInitialized = 0;


static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime*10;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
	static uint16_t ct0, ct1;
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

		uint16_t key_curr =	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);

		uint16_t i = key_state ^ ~key_curr;
		ct0 = ~( ct0 & i );
		ct1 = ct0 ^ (ct1 & i);
		i &= ct0 & ct1;
		key_state ^= i;
		key_press |= key_state & i;
	}
}
uint16_t get_key_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}

uint16_t get_key_state( uint16_t key_mask )
{
	return key_mask & key_press;
}
#define KEY_A (1<<0)

__IO uint16_t ADC3ConvertedValue = 0;

void ADC3_CH7_DMA_Config(void)
{
#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C)
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  /* DMA2 Stream0 channel2 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
  //DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&TIM4->CCR1;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC3ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC3 Channel12 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  //ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  //ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC3, &ADC_InitStructure);

  /* ADC3 regular channel7 configuration *************************************/
 // ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_28Cycles);

 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);
}


void DMA1_Stream2_IRQHandler(void)
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_TCIF2))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
 
			//usbprintf("C %u\n",tick);
  }
 
  /* Test on DMA Stream Half Transfer interrupt */
  if (DMA_GetITStatus(DMA1_Stream2, DMA_IT_HTIF2))
  {
    /* Clear DMA Stream Half Transfer interrupt pending bit */
    DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_HTIF2);
 
			//usbprintf("H\n");
  }
}

#define RXBUFFERSIZE 16
uint8_t RxBuffer [RXBUFFERSIZE];

int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);

	{
		for(int i = 0;i<RXBUFFERSIZE;i++)
		{
			RxBuffer[i]=0;
		}
		USART_InitTypeDef USART_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		DMA_InitTypeDef  DMA_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

		USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx;
		USART_Init(UART4, &USART_InitStructure);
		USART_Cmd(UART4, ENABLE);
		//USART_OverSampling8Cmd(USARTx, ENABLE); 

		NVIC_InitTypeDef NVIC_InitStructure;

		/* Configure the Priority Group to 2 bits */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

		/* Enable the UART4 RX DMA Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);


		DMA_DeInit(DMA1_Stream2);

		DMA_InitStructure.DMA_Channel = DMA_Channel_4;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // Receive
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)RxBuffer;
		DMA_InitStructure.DMA_BufferSize = (uint16_t)RXBUFFERSIZE;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

		DMA_Init(DMA1_Stream2, &DMA_InitStructure);

		/* Enable the USART Rx DMA request */
		USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);

		/* Enable DMA Stream Half Transfer and Transfer Complete interrupt */
		DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
		//DMA_ITConfig(DMA1_Stream2, DMA_IT_HT, ENABLE);

		/* Enable the DMA RX Stream */
		DMA_Cmd(DMA1_Stream2, ENABLE);

		DMA_Init(DMA1_Stream1, &DMA_InitStructure);

		/* Enable the USART Rx DMA requests */
		USART_DMACmd(UART4, USART_DMAReq_Rx , ENABLE);

		/* Enable the DMA Stream */
		DMA_Cmd(DMA1_Stream1, ENABLE);
	}



	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;       
		GPIOA->ODR           |=       (1<<0);
		GPIOA->ODR           &=       ~(1<<1);
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		//		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;       
		//		GPIO_Init(GPIOA, &GPIO_InitStructure);  
	}
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;       
		GPIO_Init(GPIOC, &GPIO_InitStructure);  
		buttonsInitialized=1;
	}
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);


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

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFC68;// this gives 50Hz exactly
	TIM_TimeBaseStructure.TIM_Prescaler = 25;
	//PrescalerValue;
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

	ADC3_CH7_DMA_Config();

	/* Start ADC3 Software Conversion */ 
	ADC_SoftwareStartConv(ADC3);


	float i = -179.99f;


	usb_serial_init();

	TimingDelay = 1;
	while(TimingDelay != 0);

		while(1)  // Do not exit
		{
			i+=20.04f;
			if(i > 180.0f )
			{

				//usbprintf("]\n");
				i=-179.99f;
			}

			for(int i = 0;i<RXBUFFERSIZE;i++)
			{
				usbprintf("%u ", RxBuffer[i]);
			}
			usbprintf("%u \n",DMA_GetCurrDataCounter(DMA1_Stream2));

			//		usbprintf("%u %u:\n",key_state,key_press);

			//3231 is 1ms exactly

			//TIM4->CCR3 = 3231+((_sinf(i)+1.0f)*1615.5f);
			//TIM4->CCR4 = 3231+((_sinf(i)+1.0f)*1615.5f);
			//TIM4->CCR1 = 3231+((_sinf(i)+1.0f)*1615.5f);
			//TIM4->CCR2 = 3231+((_sinf(i)+1.0f)*1615.5f);
			//		TIM4->CCR3 = 3231+ADC3ConvertedValue;
			//		TIM4->CCR4 = 3231+ADC3ConvertedValue;
			TIM4->CCR1 = 3231+ADC3ConvertedValue;
			TIM4->CCR2 = 3231+ADC3ConvertedValue;
			GPIOB->ODR           ^=       1<<13;

			//		if(get_key_press(KEY_A))
			{
				//Delay(1900);
				GPIOB->ODR           ^=       1<<12;

				GPIOA->ODR           &=       ~(1<<0);
				/*	GPIOA->ODR           |=       (1<<1);

					Delay(19);

					for(int i = 0;i < 5;i++)
					{
					GPIOA->ODR           &=       ~(1<<1);
					TimingDelay = 1;
					while(TimingDelay != 0);
					GPIOA->ODR           |=       (1<<1);
					TimingDelay = 1;
					while(TimingDelay != 0);
					}
					{
					Delay(20);
					GPIO_InitTypeDef GPIO_InitStructure;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
					GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
					GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
					GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;       
					GPIO_Init(GPIOA, &GPIO_InitStructure);  
					}
					*/
			}
			Delay(1000);
			//GPIOA->ODR           |=       (1<<0);
			//GPIOA->ODR           &=       ~(1<<1);
			Delay(1500);
			while(1)
			{
				for(int i = 0;i<RXBUFFERSIZE;i++)
				{
					usbprintf("%u ", RxBuffer[i]);
				}
				usbprintf("\n");
				Delay(100);
			}
		}
}

