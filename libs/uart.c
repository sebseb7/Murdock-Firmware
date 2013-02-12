
#include "uart.h"


#define RXBUFFERSIZE 16

static uint8_t rx1buffer [RXBUFFERSIZE];
static uint8_t rx2buffer [RXBUFFERSIZE];


uint8_t * get_rx1_buffer(void)
{
	return rx1buffer;
}
uint8_t * get_rx2_buffer(void)
{
	return rx2buffer;
}


void DMA1_Stream1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_Stream1, DMA_IT_TCIF1))
	{
		ch1_rx_complete();
		DMA_ClearITPendingBit(DMA1_Stream1, DMA_IT_TCIF1);
	}

}
void DMA2_Stream1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
	{
		ch2_rx_complete();
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
	}

}
	
void uart_dma_init(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA1_Stream1);
	DMA_DeInit(DMA2_Stream1);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; // Receive
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx1buffer;
	DMA_InitStructure.DMA_BufferSize = (uint16_t)RXBUFFERSIZE;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
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

	DMA_Init(DMA1_Stream1, &DMA_InitStructure);

	DMA_InitStructure.DMA_Channel = DMA_Channel_5;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)rx2buffer;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART6->DR;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	/* Enable the USART Rx DMA request */
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);

	DMA_ITConfig(DMA1_Stream1, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);

	/* Enable the DMA RX Stream */
	DMA_Cmd(DMA1_Stream1, ENABLE);
	DMA_Cmd(DMA2_Stream1, ENABLE);
}

void uart_init(void)
{
	for(int i = 0;i<RXBUFFERSIZE;i++)
	{
		rx1buffer[i]=10;
		rx2buffer[i]=11;
	}
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;
	USART_Init(USART3, &USART_InitStructure);
	USART_Init(USART6, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
	USART_Cmd(USART6, ENABLE);
	//USART_OverSampling8Cmd(USARTx, ENABLE); 

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the UART3 RX DMA Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_Init(&NVIC_InitStructure);


	uart_dma_init();
	


}
