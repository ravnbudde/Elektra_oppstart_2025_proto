///*
 //* i2c.c
 //*
 //* Created: 27.11.2024 17:26:41
 //*  Author: eikel
 //*/ 
//
//#include "i2c.h"
//
//void i2c_init(void) {
	//// Sett SDA og SCL som utganger
	//DDRB |= (1 << SDA_PIN) | (1 << SCL_PIN);
	//// Aktiver pull-up motstander
	//PORTB |= (1 << SDA_PIN) | (1 << SCL_PIN);
	//// Initialiser USI
	//USIDR = 0xFF;
	//USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK);
	//USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
//}
//
//uint8_t i2c_start(uint8_t address) {
	//// Generer startbetingelse
	//SDA_LOW();
	//_delay_us(4);
	//SCL_LOW();
	//// Send adresse
	//return i2c_write(address);
//}
//
//uint8_t i2c_write(uint8_t data) {
	//USIDR = data;
	//// Overfør 8 bits data
	//for (uint8_t i = 0; i < 8; i++) {
		//SCL_HIGH();
		//_delay_us(4);
		//SCL_LOW();
		//_delay_us(4);
	//}
	//// Les ACK
	//SCL_HIGH();
	//_delay_us(4);
	//uint8_t ack = !(SDA_READ());
	//SCL_LOW();
	//return ack;
//}
//
//uint8_t i2c_read(uint8_t ack) {
	//uint8_t data = 0;
	//// Sett SDA som inngang
	//DDRB &= ~(1 << SDA_PIN);
	//for (uint8_t i = 0; i < 8; i++) {
		//SCL_HIGH();
		//_delay_us(4);
		//data <<= 1;
		//if (SDA_READ()) {
			//data |= 1;
		//}
		//SCL_LOW();
		//_delay_us(4);
	//}
	//// Send ACK/NACK
	//DDRB |= (1 << SDA_PIN);
	//if (ack) {
		//SDA_LOW();
		//} else {
		//SDA_HIGH();
	//}
	//SCL_HIGH();
	//_delay_us(4);
	//SCL_LOW();
	//SDA_HIGH();
	//return data;
//}
//
//void i2c_stop(void) {
	//// Generer stoppbetingelse
	//SCL_LOW();
	//SDA_LOW();
	//SCL_HIGH();
	//_delay_us(4);
	//SDA_HIGH();
	//_delay_us(4);
//}
//
