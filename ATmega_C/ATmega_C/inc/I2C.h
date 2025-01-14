/*
 * I2C.h
 *
 * Created: 13.01.2025 13:32:30
 *  Author: ravnr
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "timer0.h"
#include <util/twi.h>

#define DEBUG_LOG			0
#define SUCCESS				0

typedef uint8_t bool;
#define TRUE	1
#define FALSE	0 


#define TW_SCL_PIN			PC5
#define TW_SDA_PIN			PC4

#define TW_SLA_W(ADDR)		((ADDR << 1) | TW_WRITE)
#define TW_SLA_R(ADDR)		((ADDR << 1) | TW_READ)
#define TW_READ_ACK			1
#define TW_READ_NACK		0

typedef uint16_t ret_code_t;

typedef enum {
	TW_FREQ_100K,
	TW_FREQ_250K,
	TW_FREQ_400K
} twi_freq_mode_t;

void tw_init(twi_freq_mode_t twi_freq, bool pullup_en);


ret_code_t tw_master_transmit(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool repeat_start);
ret_code_t tw_master_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len);



ret_code_t tw_start(void);
void tw_stop(void);
ret_code_t tw_write_sla(uint8_t sla);
ret_code_t tw_write(uint8_t data);
uint8_t tw_read(bool read_ack);




#endif /* I2C_H_ */