#ifndef __BMP085_H__
#define __BMP085_H__

#include "main.h"

uint8_t ak8975_getCalData(void); 								//has to be called once for initialization

void ak8975_readSensor(uint32_t * x,uint32_t * y,uint32_t * z);

#endif
