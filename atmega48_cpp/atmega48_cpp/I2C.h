/*
 * I2C.h
 *
 * Created: 11.01.2025 15:30:27
 *  Author: ravnr
 */ 





#ifndef I2C_H_
#define I2C_H_

#include "timer0.h"

#define FCPU 8000000UL

class I2C {
	public:
	// Constructor
	I2C(uint32_t clockSpeed);

	// Methods for initialization and communication
	void Start();
	uint8_t Stop();
	void Write(uint8_t data);
	uint8_t ReadAck();
	uint8_t ReadNack();
	void WriteToAddress(uint8_t address, uint8_t data);
	uint8_t ReadFromAddress(uint8_t address);
	
	void ReadMultipleFromAddress(uint8_t address, uint8_t registerAddress, uint8_t* buffer, uint8_t length);
	void WriteMultipleToAddress(uint8_t address, uint8_t registerAddress, const uint8_t* data, uint8_t length);
	void ReadMultipleFromRegister(uint8_t address, uint8_t registerAddress, uint8_t* buffer, uint8_t length);

	private:
	uint32_t clockSpeed; // Store clock speed for reference
};





#endif /* I2C_H_ */





