/*
 * vl53l0x.c
 *
 * Created: 13.01.2025 22:31:59
 *  Author: ravnr
 */ 
#include "../inc/vl53l0x.h"


#define SYSRANGE_START									0x00
#define SYSTEM_SEQUENCE_CONFIG							0x01
#define SYSTEM_INTERRUPT_CLEAR							0x0B
#define RESULT_INTERRUPT_STATUS							0x13
#define RESULT_RANGE_STATUS								0x14
#define IDENTIFICATION_MODEL_ID							0xC0
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV				0x89
#define MSRC_CONFIG_CONTROL								0x60
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT		0x44






bool VL53L0X_init(struct VL53L0X* dev)
{
	uint8_t tempint;
	
	
	// check model ID register (value specified in datasheet)
	VL53L0X_read_reg(dev, IDENTIFICATION_MODEL_ID, &tempint);
	if(tempint != 0xEE) { return FALSE; }
	

	//Setter sensor i 2.8v mode (skrur på siste (første?) biten i det registeret)
	VL53L0X_read_reg(dev, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, &tempint);
	VL53L0X_write_reg(dev, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, tempint | 0x01);
	
	
	//Her og under begynner APIen sin DataInit()
	
	//// "Set I2C standard mode"
	VL53L0X_write_reg(dev, 0x88, 0x00);

	//Alle register og verdier under er henta fra arduino-bibliotekene (forsåvidt de over her og)
	VL53L0X_write_reg(dev, 0x80, 0x01);
	VL53L0X_write_reg(dev, 0xFF, 0x01);
	VL53L0X_write_reg(dev, 0x00, 0x00);
	VL53L0X_read_reg(dev, 0x91, &dev->stop_variable);
	VL53L0X_write_reg(dev, 0x00, 0x01);
	VL53L0X_write_reg(dev, 0xFF, 0x00);
	VL53L0X_write_reg(dev, 0x80, 0x00);


	// disable SIGNAL_RATE_MSRC (bit 1) and SIGNAL_RATE_PRE_RANGE (bit 4) limit checks
	VL53L0X_read_reg(dev, MSRC_CONFIG_CONTROL, &tempint);
	VL53L0X_write_reg(dev, MSRC_CONFIG_CONTROL, tempint | 0x12);


	// SetSignalRateLimit(sr), 0<sr<512 (hvis vi skal stole på arduino bib, 0.25 er standard og ikke redigerbar)
	// Sender verdien bitshifta 7 til venstre. siden vi sender 1/4 er det samme som å bitshifte to til høyre igjen = 1<<5
	// sr er derfor lik 0.25 * (1<<7)
	uint8_t sr[2];
	sr[0] = 0;
	sr[1] = (1<<5);
	VL53L0X_write_16reg(dev, FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT, sr);

	VL53L0X_write_reg(dev, SYSTEM_SEQUENCE_CONFIG, 0xFF);

	//// DataInit() ferdig her


	//Basert på test med arduino, fungerer det greit uten mer initialisering
	//Om det evt feiler senere er funksjonene jeg har skrivd om hentet herfra:
	//https://github.com/pololu/vl53l0x-arduino/blob/master/VL53L0X.cpp
	
	return TRUE;
}

void VL53L0X_read_mm(struct VL53L0X* dev)
{
	VL53L0X_write_reg(dev, 0x80, 0x01);
	VL53L0X_write_reg(dev, 0xFF, 0x01);
	VL53L0X_write_reg(dev, 0x00, 0x00);
	VL53L0X_read_reg(dev, 0x91, &dev->stop_variable);
	VL53L0X_write_reg(dev, 0x00, 0x01);
	VL53L0X_write_reg(dev, 0xFF, 0x00);
	VL53L0X_write_reg(dev, 0x80, 0x00);

	VL53L0X_write_reg(dev, SYSRANGE_START, 0x01);
	  
	  
	//Vent til måling er ferdig  
	uint8_t tempval = 0; 
	VL53L0X_read_reg(dev, SYSRANGE_START, &tempval);
	while (tempval & 0x01)
	{
		VL53L0X_read_reg(dev, SYSRANGE_START, &tempval);
		//Legg til funksjon for timeout her er nok lurt
	}
	  
	 
	VL53L0X_read_reg(dev, RESULT_INTERRUPT_STATUS, &tempval);
	while ((tempval & 0x07) == 0)
	{
		VL53L0X_read_reg(dev, RESULT_INTERRUPT_STATUS, &tempval);
		//Legg til funksjon for timeout her er nok lurt	
	}
	
	uint8_t buffer[2];
	VL53L0X_read_16reg(dev, RESULT_RANGE_STATUS+10, buffer);
	
	dev->last_measure_mm = (buffer[0]<<8)|buffer[0];

	VL53L0X_write_reg(dev, SYSTEM_INTERRUPT_CLEAR, 0x01);
}



