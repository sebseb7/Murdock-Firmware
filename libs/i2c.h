#ifndef I2C_H_
#define I2C_H_

#include "main.h"

void i2c2_init(void);
uint16_t i2c_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
void i2c_stop(I2C_TypeDef* I2Cx);
uint8_t i2c_read_ack(I2C_TypeDef* I2Cx, uint8_t *result);
uint8_t i2c_read_nack(I2C_TypeDef* I2Cx, uint8_t *result);
uint16_t i2c_write(I2C_TypeDef* I2Cx, uint8_t data);

void i2c2_write(uint8_t addr,uint8_t reg, uint8_t byte);
int16_t i2c2_reads16(uint8_t addr,uint8_t reg);
int32_t i2c2_reads24(uint8_t addr,uint8_t reg);
uint16_t i2c2_readu16(uint8_t addr,uint8_t reg);
int16_t i2c2_read16sX(uint8_t addr,uint8_t reg);

#endif
