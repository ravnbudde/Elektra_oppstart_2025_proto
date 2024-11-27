///*
 //* i2c.h
 //*
 //* Created: 27.11.2024 17:25:07
 //*  Author: eikel
 //*/ 
//
//
//#ifndef I2C_H
//#define I2C_H
//
//#include <avr/io.h>
//#include <util/delay.h>
//
//// Definerer SDA og SCL pinner
//#define SDA_PIN PB0
//#define SCL_PIN PB2
//
//// Makroer for å sette og lese pinner
//#define SDA_HIGH() (PORTB |= (1 << SDA_PIN))
//#define SDA_LOW()  (PORTB &= ~(1 << SDA_PIN))
//#define SCL_HIGH() (PORTB |= (1 << SCL_PIN))
//#define SCL_LOW()  (PORTB &= ~(1 << SCL_PIN))
//#define SDA_READ() (PINB & (1 << SDA_PIN))
//
//void i2c_init(void);
//uint8_t i2c_start(uint8_t address);
//uint8_t i2c_write(uint8_t data);
//uint8_t i2c_read(uint8_t ack);
//void i2c_stop(void);
//
//#endif
