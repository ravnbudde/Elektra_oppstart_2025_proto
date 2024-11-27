/*
 * ds1307.h
 *
 * Created: 27.11.2024 18:25:06
 *  Author: eikel
 */ 


#ifndef DS1307_H
#define DS1307_H

#include <avr/io.h>

#define DS1307_I2C_ADDRESS 0x68 // 7-bit adresse for DS1307

void ds1307_init();
void ds1307_set_time(uint8_t seconds, uint8_t minutes, uint8_t hours);
void ds1307_set_date(uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void ds1307_get_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours);
void ds1307_get_date(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year);

#endif /* DS1307_H_ */