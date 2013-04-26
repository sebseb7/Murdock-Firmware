#include "main.h"
#include "i2c.h"

#define TIMEOUT_MAX    0x200
uint32_t i2c_timeout = TIMEOUT_MAX;


void i2c2_init(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	// enable APB1 peripheral clock for I2C2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;			
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;	
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);		

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2); // SDA

	I2C_InitStruct.I2C_ClockSpeed = 400000; 		
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode
	//I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_Init(I2C2, &I2C_InitStruct);				

	// enable I2C2
	I2C_Cmd(I2C2, ENABLE);
}



uint16_t i2c_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	i2c_timeout = TIMEOUT_MAX;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if (i2c_timeout-- == 0) {I2C_GenerateSTOP(I2Cx, ENABLE);i2c_error(1);return 1;}
	}

	I2C_GenerateSTART(I2Cx, ENABLE);

	i2c_timeout = TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if (i2c_timeout-- == 0) {I2C_GenerateSTOP(I2Cx, ENABLE);i2c_error(2);return 2;};
	}

	I2C_Send7bitAddress(I2Cx, address<<1, direction);

	i2c_timeout = TIMEOUT_MAX;
	if(direction == I2C_Direction_Transmitter)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			if (i2c_timeout-- == 0) {I2C_GenerateSTOP(I2Cx, ENABLE);i2c_error(3);return 3;};
		}
	}
	else if(direction == I2C_Direction_Receiver)
	{
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			if (i2c_timeout-- == 0) {I2C_GenerateSTOP(I2Cx, ENABLE);i2c_error(4);return 4;}
		}
	}

	return 0;
}


uint16_t i2c_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	
	i2c_timeout = TIMEOUT_MAX;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if (i2c_timeout-- == 0) {I2C_GenerateSTOP(I2Cx, ENABLE);i2c_error(5);return 5;};
	}
	return 0;

}


// api changes need to continue from here on:


uint8_t i2c_read_ack(I2C_TypeDef* I2Cx,uint8_t *result){
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	uint16_t timeout = 10000;
	while( (timeout > 0)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) )
	{
		timeout--;
	}
	if(timeout==0)
	{
		I2C_GenerateSTOP(I2Cx, ENABLE);
		i2c_error(6);
		return 0;
	}
	*result = I2C_ReceiveData(I2Cx);
	return 1;
}

uint8_t i2c_read_nack(I2C_TypeDef* I2Cx,uint8_t *result ){
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	uint16_t timeout = 10000;
	while( (timeout > 0)&&(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) )
	{
		timeout--;
	}
	if(timeout==0)
	{
		I2C_GenerateSTOP(I2Cx, ENABLE);
		i2c_error(7);
		return 0;
	}
	*result = I2C_ReceiveData(I2Cx);
	return 1;
}

void i2c_stop(I2C_TypeDef* I2Cx){
	I2C_GenerateSTOP(I2Cx, ENABLE);
}

void i2c2_write(uint8_t addr,uint8_t reg, uint8_t byte)
{
	i2c_start(I2C2, addr, I2C_Direction_Transmitter); 
	i2c_write(I2C2, reg); 
	i2c_write(I2C2, byte); 
	i2c_stop(I2C2); 
}

int16_t i2c2_reads16(uint8_t addr,uint8_t reg)
{
	i2c_start(I2C2, addr, I2C_Direction_Transmitter); 
	i2c_write(I2C2, reg); 
	i2c_stop(I2C2); 

	uint8_t received_data[2];

	i2c_start(I2C2, addr, I2C_Direction_Receiver);
	i2c_read_ack(I2C2,&received_data[0]); 
	i2c_read_nack(I2C2,&received_data[1]); 
	i2c_stop(I2C2); 

	return received_data[0] << 8 | received_data[1];
}

int32_t i2c2_reads24(uint8_t addr,uint8_t reg)
{
	i2c_start(I2C2, addr, I2C_Direction_Transmitter); 
	i2c_write(I2C2, reg); 
	i2c_stop(I2C2); 

	uint8_t received_data[3];

	i2c_start(I2C2, addr, I2C_Direction_Receiver);
	i2c_read_ack(I2C2,&received_data[0]); 
	i2c_read_ack(I2C2,&received_data[1]); 
	i2c_read_nack(I2C2,&received_data[2]); 
	i2c_stop(I2C2); 

	return (received_data[0] << 16) | (received_data[1] << 8) | received_data[2];
}


uint16_t i2c2_readu16(uint8_t addr,uint8_t reg)
{
	i2c_start(I2C2, addr, I2C_Direction_Transmitter); 
	i2c_write(I2C2, reg); 
	i2c_stop(I2C2); 

	uint8_t received_data[2];

	i2c_start(I2C2, addr, I2C_Direction_Receiver);
	i2c_read_ack(I2C2,&received_data[0]); 
	i2c_read_nack(I2C2,&received_data[1]); 
	i2c_stop(I2C2); 

	return received_data[0] << 8 | received_data[1];
}

int16_t i2c2_read16sX(uint8_t addr,uint8_t reg)
{
	i2c_start(I2C2, addr, I2C_Direction_Transmitter); 
	i2c_write(I2C2, reg); 
	i2c_stop(I2C2); 

	uint8_t received_data[2];

	i2c_start(I2C2, addr, I2C_Direction_Receiver);
	i2c_read_ack(I2C2,&received_data[0]); 
	i2c_read_nack(I2C2,&received_data[1]); 
	i2c_stop(I2C2); 

	return received_data[1] << 8 | received_data[0];
}

