/*
 * I2C.h
 *
 * Created: 11.01.2025 15:30:27
 *  Author: ravnr
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#define FCPU 8000000UL

void I2C_Init(uint32_t clockSpeed);

void I2C_Start();
void I2C_Stop();

void I2C_Write(uint8_t data);
uint8_t I2C_ReadAck();
uint8_t I2C_ReadNack();

void I2C_WriteToAddress(uint8_t address, uint8_t data);
uint8_t I2C_ReadFromAddress(uint8_t address);





#endif /* I2C_H_ */