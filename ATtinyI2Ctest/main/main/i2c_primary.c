#include "i2c_primary.h"

void i2c_init() {
	// Konfigurer SDA og SCL som utganger
	DDRB |= (1 << PIN_SDA);
	DDRB |= (1 << PIN_SCL);

	// Konfigurer LED som utgang
	DDRB |= (1 << PIN_LED);
	PORTB &= ~(1 << PIN_LED); // Slå av LED

	// Sett SDA og SCL høyt
	PORTB |= (1 << PIN_SDA);
	PORTB |= (1 << PIN_SCL);

	// Initialiser USI-register
	USIDR = 0xFF;
	USICR = (1 << USIWM1) | (1 << USICS1) | (1 << USICLK);
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) |
	(1 << USIDC) | (0x0 << USICNT0);
}

void i2c_start() {
	// Slå på LED for å indikere start
	PORTB |= (1 << PIN_LED);

	// Generer startbetingelse
	PORTB |= (1 << PIN_SDA); // SDA høyt
	PORTB |= (1 << PIN_SCL); // SCL høyt
	while (!(PORTB & (1 << PIN_SCL))); // Vent til SCL er høy

	PORTB &= ~(1 << PIN_SDA); // SDA lav (startbetingelse)
	_delay_us(WAIT_LONG);

	PORTB &= ~(1 << PIN_SCL); // SCL lav
}

void i2c_stop() {
	// SDA lav
	PORTB &= ~(1 << PIN_SDA);

	// Frigi SCL
	PORTB |= (1 << PIN_SCL);
	while (!(PINB & (1 << PIN_SCL)));

	_delay_us(WAIT_LONG);

	// Frigi SDA
	PORTB |= (1 << PIN_SDA);

	// Slå av LED for å indikere stopp
	PORTB &= ~(1 << PIN_LED);
	_delay_us(WAIT_SHORT);
}

unsigned char i2c_transfer(unsigned char usisr_mask) {
	PORTB &= ~(1 << PIN_SCL); // SCL lav

	USISR = usisr_mask;

	do {
		_delay_us(WAIT_LONG);
		USICR |= (1 << USITC);
		while (!(PINB & (1 << PIN_SCL))); // Vent til SCL er høy
		_delay_us(WAIT_SHORT);
		USICR |= (1 << USITC);
	} while (!(USISR & (1 << USIOIF)));
	_delay_us(WAIT_LONG);

	// Frigi SDA
	unsigned char temp = USIDR;
	USIDR = 0xFF;

	return temp;
}

unsigned char i2c_write_byte(unsigned char data) {
	USIDR = data;
	i2c_transfer(USISR_CLOCK_8_BITS);

	DDRB &= ~(1 << PIN_SDA); // Sett SDA som inngang
	unsigned char nack = i2c_transfer(USISR_CLOCK_1_BIT);
	DDRB |= (1 << PIN_SDA); // Sett SDA som utgang

	return nack;
}

unsigned char i2c_read_byte(unsigned char nack) {
	DDRB &= ~(1 << PIN_SDA); // Sett SDA som inngang
	unsigned char data = i2c_transfer(USISR_CLOCK_8_BITS);
	DDRB |= (1 << PIN_SDA); // Sett SDA som utgang

	// Send NACK
	USIDR = nack;
	i2c_transfer(USISR_CLOCK_1_BIT);

	return data;
}
