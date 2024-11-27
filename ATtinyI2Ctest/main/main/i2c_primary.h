
#include "avr/io.h"
#include "avr/interrupt.h"
#define F_CPU 8000000UL 
#include <util/delay.h>

#define PIN_SDA 				PB0
#define PIN_SCL 				PB2

#define WAIT_LONG			40	//5 // 4,7us
#define WAIT_SHORT 			30	//4 // 4,0us

// USISR mask
#define USISR_CLOCK_8_BITS		0b11110000
#define USISR_CLOCK_1_BIT  		0b11111110

#define READ					1
#define WRITE					0

void i2c_init();

void i2c_start();

void i2c_stop();

unsigned char i2c_transfer(unsigned char usisr_mask);

unsigned char i2c_write_byte(unsigned char data);

unsigned char i2c_read_byte(unsigned char nack);

unsigned char i2c_write_address(unsigned char data, unsigned char RW);