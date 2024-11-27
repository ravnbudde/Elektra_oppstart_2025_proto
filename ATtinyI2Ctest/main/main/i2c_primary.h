#ifndef I2C_PRIMARY_H
#define I2C_PRIMARY_H

#include <avr/io.h>
#include <util/delay.h>

// Konfigurasjon for I2C-pins
#define PIN_SDA                PB0
#define PIN_SCL                PB2

// Konfigurasjon for LED-indikator
#define PIN_LED                PB1

// Tidskonfigurasjon for I2C
#define WAIT_LONG              5 // 4,7us
#define WAIT_SHORT             4 // 4,0us

// USISR-masker
#define USISR_CLOCK_8_BITS     0b11110000
#define USISR_CLOCK_1_BIT      0b11111110

// Funksjonsprototyper
void i2c_init();
void i2c_start();
void i2c_stop();
unsigned char i2c_transfer(unsigned char usisr_mask);
unsigned char i2c_write_byte(unsigned char data);
unsigned char i2c_read_byte(unsigned char nack);

#endif
