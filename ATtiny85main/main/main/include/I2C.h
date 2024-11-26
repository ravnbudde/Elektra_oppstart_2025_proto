/*
 * I2C.h
 *
 * Created: 26/11/2024 12:33:12
 *  Author: Ravn
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer1.h"

#define SDA PB0 
#define SCL PB2 

/*NB: 
	litt usikker her p? om int flaggene i USISR faktisk leses som 1 eller 0 n?r de er flagga (tror 1, er bare forvirrende at man ogs? skriver 1 til de for ? cleare)?? m? sjekkes
	Adresser sendes som 7 bit. read/write legges til i funksjonene, s? ikke legg inn manuelt, bare send 7-bit adresse som uint8 med MSB som 0 (eller 1 har vel ikke noe ? si)
*/

void init_i2c_master(void);
void i2c_start_condition(void);
void i2c_stop_condition(void);
uint8_t i2c_recieve_ack(void);
void i2c_send_byte(uint8_t data);
void i2c_write(uint8_t addr, uint8_t data);
uint8_t i2c_read_byte(uint8_t send_ack);
void i2c_read(uint8_t addr, uint8_t *buffer, uint8_t length);




#endif /* I2C_H_ */