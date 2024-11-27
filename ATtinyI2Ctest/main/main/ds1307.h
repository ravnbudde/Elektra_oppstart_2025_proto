/*
 * ds1307.h
 *
 * Created: 27.11.2024 18:25:06
 *  Author: eikel
 */ 


#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

#define DS1307_ADDRESS 0x68

void ds1307_init();
void ds1307_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void ds1307_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

#endif /* DS1307_H_ */