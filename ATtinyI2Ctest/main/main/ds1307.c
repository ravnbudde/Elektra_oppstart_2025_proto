/*
 * ds1307.c
 *
 * Created: 27.11.2024 18:25:21
 *  Author: eikel
 */ 
#include "ds1307.h"
#include "i2c_primary.h"

// Konverter BCD til binær
static uint8_t bcd_to_binary(uint8_t bcd) {
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Konverter binær til BCD
static uint8_t binary_to_bcd(uint8_t binary) {
	return ((binary / 10) << 4) | (binary % 10);
}

void ds1307_init() {
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1)); // Skrive
	i2c_write_byte(0x00); // Sekund-register
	i2c_write_byte(0x00); // Sett oscillator bit CH = 0
	i2c_stop();
}

void ds1307_set_time(uint8_t seconds, uint8_t minutes, uint8_t hours) {
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1)); // Skrive
	i2c_write_byte(0x00); // Start frå sekund-register
	i2c_write_byte(binary_to_bcd(seconds));
	i2c_write_byte(binary_to_bcd(minutes));
	i2c_write_byte(binary_to_bcd(hours) & 0x3F); // 24-timers format
	i2c_stop();
}

void ds1307_set_date(uint8_t day, uint8_t date, uint8_t month, uint8_t year) {
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1)); // Skrive
	i2c_write_byte(0x03); // Start frå dag-register
	i2c_write_byte(binary_to_bcd(day));
	i2c_write_byte(binary_to_bcd(date));
	i2c_write_byte(binary_to_bcd(month));
	i2c_write_byte(binary_to_bcd(year));
	i2c_stop();
}

void ds1307_get_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours) {
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1)); // Skrive
	i2c_write_byte(0x00); // Start frå sekund-register
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1) | 1); // Lese
	*seconds = bcd_to_binary(i2c_read_byte(1));
	*minutes = bcd_to_binary(i2c_read_byte(1));
	*hours = bcd_to_binary(i2c_read_byte(0));
	i2c_stop();
}

void ds1307_get_date(uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *year) {
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1)); // Skrive
	i2c_write_byte(0x03); // Start frå dag-register
	i2c_start();
	i2c_write_byte((DS1307_I2C_ADDRESS << 1) | 1); // Lese
	*day = bcd_to_binary(i2c_read_byte(1));
	*date = bcd_to_binary(i2c_read_byte(1));
	*month = bcd_to_binary(i2c_read_byte(1));
	*year = bcd_to_binary(i2c_read_byte(0));
	i2c_stop();
}
