/*
 * vl53l0x_i2c_platform.h
 *
 * Created: 13.01.2025 13:32:45
 *  Author: ravnr
 */ 


#ifndef VL53L0X_I2C_PLATFORM_H_
#include "I2C.h"

struct VL53L0X {
	uint8_t address;
	uint8_t stop_variable;
	uint8_t last_status;
};



void VL53L0X_write_mult(struct VL53L0X* dev, uint8_t reg, uint8_t* data, uint8_t count, bool repeat_start);
void VL53L0X_read_mult(struct VL53L0X* dev, uint8_t reg, uint8_t* buffer, uint8_t count);

void VL53L0X_read_reg(struct VL53L0X* dev, uint8_t reg, uint8_t* buffer);
void VL53L0X_write_reg(struct VL53L0X* dev, uint8_t reg, uint8_t data);


#endif /* VL53L0X_I2C_PLATFORM_H_ */