/*
 * i2c.h
 *
 * Created: 27.11.2024 17:25:07
 *  Author: eikel
 */ 


#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

// Makroer for SDA og SCL
#define SDA_HIGH (PORTB |= (1 << PB0))
#define SDA_LOW (PORTB &= ~(1 << PB0))
#define SCL_HIGH (PORTB |= (1 << PB2))
#define SCL_LOW (PORTB &= ~(1 << PB2))

void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write(uint8_t data);
uint8_t i2c_read(uint8_t ack);

#endif

