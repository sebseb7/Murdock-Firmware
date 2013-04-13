
#include "sbus.h"


#define RXSBUSBUFFERSIZE 25

static uint8_t sbusbuffer [RXSBUSBUFFERSIZE];


uint8_t * get_rx_sbus_buffer(void)
{
	return sbusbuffer;
}

void DMA2_Stream5_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5))
	{
		DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
		sbus_rx_complete();
	}

}
	

void sbus_init(void)
{
	for(int i = 0;i<RXSBUSBUFFERSIZE;i++)
	{
		sbusbuffer[i]=12;
	}

	{
		//usart2: a3
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;       
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
	{
		USART_InitTypeDef USART_InitStructure;

		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		USART_InitStructure.USART_BaudRate = 100000;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx;
		USART_Init(USART1, &USART_InitStructure);

		USART_Cmd(USART1, ENABLE);
		//USART_OverSampling8Cmd(USARTx, ENABLE); 
	}

	{
		NVIC_InitTypeDef NVIC_InitStructure;

		/* Configure the Priority Group to 2 bits */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

		/* Enable the UART2 RX DMA Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	sbus_dma_init();
}
	
void sbus_dma_init(void)
{
	{
		DMA_InitTypeDef  DMA_InitStructure;
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_DeInit(DMA2_Stream5);

		DMA_InitStructure.DMA_Channel = DMA_Channel_4;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // Receive
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sbusbuffer;
		DMA_InitStructure.DMA_BufferSize = (uint16_t)RXSBUSBUFFERSIZE;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//usart2
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

		DMA_Init(DMA2_Stream5, &DMA_InitStructure);


		/* Enable the USART Rx DMA request */
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);//usart2

		DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);

		/* Enable the DMA RX Stream */
		DMA_Cmd(DMA2_Stream5, ENABLE);
		
		
		
		
		//DMA_InitTypeDef  DMA_InitStructure;
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
		//DMA_DeInit(DMA1_Stream5);

		//DMA_InitStructure.DMA_Channel = DMA_Channel_4;
		//DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // Receive
		//DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sbusbuffer;
		//DMA_InitStructure.DMA_BufferSize = (uint16_t)RXSBUSBUFFERSIZE;
		//DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;//usart2
		//DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		//DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		//DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		//DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		//DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		//DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		//DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

		//DMA_Init(DMA1_Stream5, &DMA_InitStructure);


		/* Enable the USART Rx DMA request */
		//USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

		//DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

		/* Enable the DMA RX Stream */
		//DMA_Cmd(DMA1_Stream5, ENABLE);
	}


}
