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


void I2C_Init(uint32_t clockSpeed) {
	// Sett TWI bitrate
	DDRC &= ~((1<<DDC4)|(1<<DDC5));
	PORTC |= (1<<PB4)|(1<<PB5);
	
	uint8_t prescaler = 0;
	TWSR = prescaler; // Prescaler: 0
	TWBR = (int)(((8000000 / clockSpeed) - 16) / 2);


	// Aktiver TWI
	TWCR = (1 << TWEN);
}



void I2C_Start() {
	// Send START-condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1 << TWINT))); // Vent til START er sendt
	
}



void I2C_Stop() {

	// Send STOP-condition
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);

}



void I2C_Write(uint8_t data) {

	TWDR = data; // Last data til Data Register

	TWCR = (1 << TWEN) | (1 << TWINT); // Start overf?ring

	while (!(TWCR & (1 << TWINT))); // Vent til overf?ring er ferdig
	
	
	

}



uint8_t I2C_ReadAck() {

	// Lese en byte og sende ACK

	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);

	while (!(TWCR & (1 << TWINT))); // Vent til mottak er ferdig

	return TWDR;

}



uint8_t I2C_ReadNack() {

	// Lese en byte og sende NACK

	TWCR = (1 << TWEN) | (1 << TWINT);

	while (!(TWCR & (1 << TWINT))); // Vent til mottak er ferdig

	return TWDR;

}



void I2C_WriteToAddress(uint8_t address, uint8_t data) {

	I2C_Start();

	I2C_Write((address << 1) | 0); // Skriv adresse + WRITE

	I2C_Write(data); // Skriv data

	I2C_Stop();

}



uint8_t I2C_ReadFromAddress(uint8_t address) {

	uint8_t receivedData;

	I2C_Start();

	I2C_Write((address << 1) | 1); // Skriv adresse + READ
	

	receivedData = I2C_ReadNack(); // Lese data

	I2C_Stop();

	return receivedData;

}