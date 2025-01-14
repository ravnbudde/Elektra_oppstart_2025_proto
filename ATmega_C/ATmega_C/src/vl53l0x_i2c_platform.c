/*
 * vl53l0x_i2c_platform.c
 *
 * Created: 13.01.2025 13:33:53
 *  Author: ravnr
 */ 
#include "../inc/vl53l0x_i2c_platform.h"


void VL53L0X_write_mult(struct VL53L0X* dev, uint8_t reg, uint8_t* data, uint8_t count, bool repeat_start)
{
	/*_____Start Condition_____*/
	dev->last_status = tw_start();
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_start();
	}
	
	/*_____Send adresse + write_____*/
	dev->last_status = tw_write((dev->address << 1)|TW_WRITE);
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_write((dev->address << 1)|TW_WRITE);	
	}
	
	/*_____Send register_____*/
	dev->last_status = tw_write(reg);
	if (dev->last_status != SUCCESS)
	{
		dev->last_status = tw_write(reg);	
	}
	
	
	/*_____Send dataen_____*/
	for (uint8_t i = 0; i < count; ++i)
	{
		dev->last_status = tw_write(data[i]);
		if(dev->last_status != SUCCESS)
		{
			dev->last_status = tw_write(data[i]);
		}
	}
	
	
	/*_____Send stop om ikke repeated start_____*/
	if (repeat_start == FALSE)
	{
		tw_stop();
	}
	
}
void VL53L0X_read_mult(struct VL53L0X* dev, uint8_t reg, uint8_t* buffer, uint8_t count)
{
	/*_____Start Condition_____*/
	dev->last_status = tw_start();
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_start();
	}
		
	/*_____Send adresse + write_____*/
	dev->last_status = tw_write((dev->address << 1)|TW_WRITE);
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_write((dev->address << 1)|TW_WRITE);
	}
		
	/*_____Send register_____*/
	dev->last_status = tw_write(reg);
	if (dev->last_status != SUCCESS)
	{
		dev->last_status = tw_write(reg);
	}
	
	/*_____Send repeat start_____*/
	dev->last_status = tw_start();
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_start();
	}
	
	/*_____Send adresse + read_____*/
	dev->last_status = tw_write((dev->address << 1)|TW_READ);
	if(dev->last_status != SUCCESS)
	{
		dev->last_status = tw_write((dev->address << 1)|TW_READ);
	}
	
		
	/*_____Les dataen_____*/
	for (uint8_t i = 0; i < count-1; ++i)
	{
		buffer[i] = tw_read(TW_READ_ACK); //med ACK siden den forventer fler
	}
	buffer[count-1] = tw_read(TW_READ_NACK); //Sender NACK på siste
		
		
	/*_____Send stop_____*/
	tw_stop();
	
}


void VL53L0X_read_reg(struct VL53L0X* dev, uint8_t reg, uint8_t* buffer)
{
	VL53L0X_read_mult(dev, reg, buffer, 1);
}
void VL53L0X_write_reg(struct VL53L0X* dev, uint8_t reg, uint8_t data)
{
	VL53L0X_write_mult(dev, reg, &data, 1, FALSE);
}
void VL53L0X_read_16reg(struct VL53L0X* dev, uint8_t reg, uint8_t* buffer)
{
	VL53L0X_read_mult(dev, reg, buffer, 2);
}
void VL53L0X_write_16reg(struct VL53L0X* dev, uint8_t reg, uint8_t* data)
{
	VL53L0X_write_mult(dev, reg, &data, 2, FALSE);
}


