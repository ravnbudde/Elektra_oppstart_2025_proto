/*
 * i2c.c
 *
 * Created: 27.11.2024 17:26:41
 *  Author: eikel
 */ 

#include "i2c.h"
#include <avr/io.h>
#include <util/delay.h>

void i2c_init() {
	// Sett opp SCL og SDA som utganger
	DDRB |= (1 << PB0) | (1 << PB2); // SCL på PB2, SDA på PB0
	PORTB |= (1 << PB0) | (1 << PB2); // Aktiver pull-up
}

void i2c_start() {
	// START condition
	SDA_HIGH;
	SCL_HIGH;
	_delay_us(4);
	SDA_LOW;
	_delay_us(4);
	SCL_LOW;
}

void i2c_stop() {
	// STOP condition
	SCL_LOW;
	SDA_LOW;
	_delay_us(4);
	SCL_HIGH;
	_delay_us(4);
	SDA_HIGH;
}

void i2c_write(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80) {
			SDA_HIGH;
			} else {
			SDA_LOW;
		}
		data <<= 1;
		SCL_HIGH;
		_delay_us(2);
		SCL_LOW;
		_delay_us(2);
	}
	// ACK-bit
	SDA_HIGH;
	SCL_HIGH;
	_delay_us(2);
	SCL_LOW;
}

uint8_t i2c_read(uint8_t ack) {
	uint8_t data = 0;
	SDA_HIGH; // Sett linjen som inngang
	for (uint8_t i = 0; i < 8; i++) {
		data <<= 1;
		SCL_HIGH;
		if (PINB & (1 << PB0)) {
			data |= 1;
		}
		SCL_LOW;
	}
	// Send ACK/NACK
	if (ack) {
		SDA_LOW;
		} else {
		SDA_HIGH;
	}
	SCL_HIGH;
	_delay_us(2);
	SCL_LOW;
	return data;
}
