/*
 * ds1307.c
 *
 * Created: 27.11.2024 18:25:21
 *  Author: eikel
 */ 

#include "ds1307.h"
#include "i2c.h"

void ds1307_init() {
	i2c_start();
	i2c_write(DS1307_ADDRESS << 1); // Skrivebit
	i2c_write(0x00); // Pek på sekunder-registeret
	i2c_write(0x00); // Sett klokka til å starte
	i2c_stop();
}

void ds1307_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	i2c_start();
	i2c_write(DS1307_ADDRESS << 1); // Skrivebit
	i2c_write(0x00); // Pek på sekunder-registeret
	i2c_write(seconds);
	i2c_write(minutes);
	i2c_write(hours);
	i2c_stop();
}

void ds1307_get_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
	i2c_start();
	i2c_write(DS1307_ADDRESS << 1); // Skrivebit
	i2c_write(0x00); // Pek på sekunder-registeret
	i2c_start();
	i2c_write((DS1307_ADDRESS << 1) | 1); // Lesebit
	*seconds = i2c_read(1);
	*minutes = i2c_read(1);
	*hours = i2c_read(0);
	i2c_stop();
}
