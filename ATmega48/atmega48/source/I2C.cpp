/*
 * I2C.cpp
 *
 * Created: 11.01.2025 15:30:43
 *  Author: ravnr
 */ 

#include "../include/I2C.h"



/*_______Litt inspirasjon fra datablad_________*/  
//(side 232)



/*_______Chat sitt forslag på I2C-funskjoner_________*/  
//Ikke tid til å se over nå, må gjøres + testes


I2C::I2C(uint32_t clockSpeed) {
	// Sett TWI bitrate
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	PORTC |= (1<<PB4)|(1<<PB5);
	
	uint8_t prescaler = 0;
	TWSR = prescaler; // Prescaler: 0
	TWBR = (int)(((8000000 / clockSpeed) - 16) / 2);


	// Aktiver TWI
	TWCR = (1 << TWEN);
}



void I2C::Start() {
	// Send START-condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1 << TWINT))); // Vent til START er sendt
	
}



uint8_t I2C::Stop() {

	// Send STOP-condition
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	return TWDR;

}



void I2C::Write(uint8_t data) {

	TWDR = data; // Last data til Data Register

	TWCR = (1 << TWEN) | (1 << TWINT); // Start overf?ring

	while (!(TWCR & (1 << TWINT))); // Vent til overf?ring er ferdig
	
	//return (((TWSR & 0xF8) == 0x28) ^ 1);
}



uint8_t I2C::ReadAck() {

	// Lese en byte og sende ACK

	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT))); // Vent til mottak er ferdig

	return TWDR;

}



uint8_t I2C::ReadNack() {

	// Lese en byte og sende NACK

	TWCR = (1 << TWEN) | (1 << TWINT);

	while (!(TWCR & (1 << TWINT))); // Vent til mottak er ferdig

	return TWDR;

}



void I2C::WriteToAddress(uint8_t address, uint8_t data) {

	this->Start();

	this->Write((address << 1) | 0); // Skriv adresse + WRITE

	this->Write(data); // Skriv data

	this->Stop();

}



uint8_t I2C::ReadFromAddress(uint8_t address) {

	uint8_t receivedData;

	this->Start();

	this->Write((address << 1) | 1); // Skriv adresse + READ
	

	receivedData = this->ReadNack(); // Lese data

	this->Stop();

	return receivedData;

}

void I2C::ReadMultipleFromAddress(uint8_t address, uint8_t registerAddress, uint8_t* buffer, uint8_t length) {
	// Start condition
	this->Start();

	// Send device address with WRITE (LSB = 0)
	this->Write((address << 1) | 0);

	// Send register address
	this->Write(registerAddress);

	// Restart condition
	this->Start();

	// Send device address with READ (LSB = 1)
	this->Write((address << 1) | 1);

	// Read bytes into buffer
	for (uint8_t i = 0; i < length; i++) {
		if (i == length - 1) {
			// Send NACK after the last byte
			buffer[i] = this->ReadNack();
			} else {
			// Send ACK for intermediate bytes
			buffer[i] = this->ReadAck();
		}
	}

	// Stop condition
	this->Stop();
}

void I2C::WriteMultipleToAddress(uint8_t address, uint8_t registerAddress, const uint8_t* data, uint8_t length) {
	// Start condition
	this->Start();

	// Send device address with WRITE (LSB = 0)
	this->Write((address << 1) | 0);

	// Send register address
	this->Write(registerAddress);

	// Write data bytes from the array
	for (uint8_t i = 0; i < length; i++) {
		this->Write(data[i]);
	}

	// Stop condition
	this->Stop();
}

void I2C::ReadMultipleFromRegister(uint8_t address, uint8_t registerAddress, uint8_t* buffer, uint8_t length) {
	// Start condition
	this->Start();

	// Send device address with WRITE (LSB = 0)
	this->Write((address << 1) | 0);

	// Send register address
	this->Write(registerAddress);

	// Restart condition
	this->Start();

	// Send device address with READ (LSB = 1)
	this->Write((address << 1) | 1);

	// Read bytes into buffer
	for (uint8_t i = 0; i < length; i++) {
		if (i == length - 1) {
			// Send NACK after the last byte
			buffer[i] = this->ReadNack();
			} else {
			// Send ACK for intermediate bytes
			buffer[i] = this->ReadAck();
		}
	}

	// Stop condition
	this->Stop();
}



