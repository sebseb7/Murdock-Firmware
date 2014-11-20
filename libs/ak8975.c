#include <math.h>
#include "ak8975.h"
#include "i2c.h"


#define REG_DEVICEID      0x00
#define REG_INFO          0x01
#define REG_STATUS_1      0x02
#define REG_HXL           0x03
#define REG_HXH           0x04
#define REG_HYL           0x05
#define REG_HYH           0x06
#define REG_HZL           0x07
#define REG_HZH           0x08 


#define AK8975_I2C_ADDR 0x0c

uint8_t ak8975_getCalData(void)
{
	i2c_start(I2C2, AK8975_I2C_ADDR, I2C_Direction_Transmitter); 
	i2c_write(I2C2, REG_DEVICEID); 
	i2c_stop(I2C2); 
	

	i2c_start(I2C2, AK8975_I2C_ADDR, I2C_Direction_Receiver);

	uint8_t result = 0;
	i2c_read_ack(I2C2,&result);

	i2c_stop(I2C2); 
	return result;
}

void ak8975_readSensor(uint32_t * x,uint32_t * y,uint32_t * z)
{
	*x = i2c2_reads16(AK8975_I2C_ADDR,REG_HXL);
	*y = i2c2_reads16(AK8975_I2C_ADDR,REG_HYL);
	*z = i2c2_reads16(AK8975_I2C_ADDR,REG_HZL);
}

