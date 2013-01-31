
#include "pwm.h"


//
// servo output on B6 B7 B8 B9
//
// update:
//	TIM4->CCR3 = 3231+((_sinf(i)+1.0f)*1615.5f);
//	TIM4->CCR4 = 3231+((_sinf(i)+1.0f)*1615.5f);
//  TIM4->CCR1 = 3231+((_sinf(i)+1.0f)*1615.5f);
//  TIM4->CCR2 = 3231+((_sinf(i)+1.0f)*1615.5f);
// 
//



void PWM_Init(void)
{
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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
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
}
