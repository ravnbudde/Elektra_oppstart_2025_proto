/*
 * vl53l0x.c
 *
 * Created: 13.01.2025 22:31:59
 *  Author: ravnr
 */ 
#include "../inc/vl53l0x.h"


#define IDENTIFICATION_MODEL_ID					0xC0
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV		0x89
#define MSRC_CONFIG_CONTROL						0x60


bool VL53L0X_init(struct VL53L0X* dev)
{
	uint8_t tempint;
	
	
	// check model ID register (value specified in datasheet)
	VL53L0X_read_reg(dev, IDENTIFICATION_MODEL_ID, &tempint);
	if(tempint != 0xEE) { return FALSE; }
	

	//Setter sensor i 2.8v mode (skrur på siste (første?) biten i det registeret)
	VL53L0X_read_reg(dev, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, &tempint);
	VL53L0X_write_reg(dev, VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV, tempint | 0x01);
	
	
	// "Set I2C standard mode"
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


	//// set final range signal rate limit to 0.25 MCPS (million counts per second)
	//setSignalRateLimit(0.25);
//
	//WriteRegister(SYSTEM_SEQUENCE_CONFIG, 0xFF);
//
	//// VL53L0X_DataInit() end
//
//
//
//
	//// VL53L0X_StaticInit() begin
//
	//uint8_t spad_count;
	//bool spad_type_is_aperture;
	//if (!getSpadInfo(&spad_count, &spad_type_is_aperture)) { return false; }
//
	//// The SPAD map (RefGoodSpadMap) is read by VL53L0X_get_info_from_device() in
	//// the API, but the same data seems to be more easily readable from
	//// GLOBAL_CONFIG_SPAD_ENABLES_REF_0 through _6, so read it from there
	//uint8_t ref_spad_map[6];
	//ReadMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);
//
	//// -- VL53L0X_set_reference_spads() begin (assume NVM values are valid)
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
	//WriteRegister(DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);
//
	//uint8_t first_spad_to_enable = spad_type_is_aperture ? 12 : 0; // 12 is the first aperture spad
	//uint8_t spads_enabled = 0;
//
	//for (uint8_t i = 0; i < 48; i++)
	//{
		//if (i < first_spad_to_enable || spads_enabled == spad_count)
		//{
			//// This bit is lower than the first one that should be enabled, or
			//// (reference_spad_count) bits have already been enabled, so zero this bit
			//ref_spad_map[i / 8] &= ~(1 << (i % 8));
		//}
		//else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
		//{
			//spads_enabled++;
		//}
	//}
//
	//WriteMulti(GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);
//
	//// -- VL53L0X_set_reference_spads() end
//
	//// -- VL53L0X_load_tuning_settings() begin
	//// DefaultTuningSettings from vl53l0x_tuning.h
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x00, 0x00);
//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x09, 0x00);
	//WriteRegister(0x10, 0x00);
	//WriteRegister(0x11, 0x00);
//
	//WriteRegister(0x24, 0x01);
	//WriteRegister(0x25, 0xFF);
	//WriteRegister(0x75, 0x00);
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x4E, 0x2C);
	//WriteRegister(0x48, 0x00);
	//WriteRegister(0x30, 0x20);
//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x30, 0x09);
	//WriteRegister(0x54, 0x00);
	//WriteRegister(0x31, 0x04);
	//WriteRegister(0x32, 0x03);
	//WriteRegister(0x40, 0x83);
	//WriteRegister(0x46, 0x25);
	//WriteRegister(0x60, 0x00);
	//WriteRegister(0x27, 0x00);
	//WriteRegister(0x50, 0x06);
	//WriteRegister(0x51, 0x00);
	//WriteRegister(0x52, 0x96);
	//WriteRegister(0x56, 0x08);
	//WriteRegister(0x57, 0x30);
	//WriteRegister(0x61, 0x00);
	//WriteRegister(0x62, 0x00);
	//WriteRegister(0x64, 0x00);
	//WriteRegister(0x65, 0x00);
	//WriteRegister(0x66, 0xA0);
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x22, 0x32);
	//WriteRegister(0x47, 0x14);
	//WriteRegister(0x49, 0xFF);
	//WriteRegister(0x4A, 0x00);
//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x7A, 0x0A);
	//WriteRegister(0x7B, 0x00);
	//WriteRegister(0x78, 0x21);
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x23, 0x34);
	//WriteRegister(0x42, 0x00);
	//WriteRegister(0x44, 0xFF);
	//WriteRegister(0x45, 0x26);
	//WriteRegister(0x46, 0x05);
	//WriteRegister(0x40, 0x40);
	//WriteRegister(0x0E, 0x06);
	//WriteRegister(0x20, 0x1A);
	//WriteRegister(0x43, 0x40);
//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x34, 0x03);
	//WriteRegister(0x35, 0x44);
//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x31, 0x04);
	//WriteRegister(0x4B, 0x09);
	//WriteRegister(0x4C, 0x05);
	//WriteRegister(0x4D, 0x04);
	//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x44, 0x00);
	//WriteRegister(0x45, 0x20);
	//WriteRegister(0x47, 0x08);
	//WriteRegister(0x48, 0x28);
	//WriteRegister(0x67, 0x00);
	//WriteRegister(0x70, 0x04);
	//WriteRegister(0x71, 0x01);
	//WriteRegister(0x72, 0xFE);
	//WriteRegister(0x76, 0x00);
	//WriteRegister(0x77, 0x00);
	//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x0D, 0x01);
	//
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x80, 0x01);
	//WriteRegister(0x01, 0xF8);
	//
	//WriteRegister(0xFF, 0x01);
	//WriteRegister(0x8E, 0x01);
	//WriteRegister(0x00, 0x01);
	//WriteRegister(0xFF, 0x00);
	//WriteRegister(0x80, 0x00);
//
	//// -- VL53L0X_load_tuning_settings() end
//
	//// "Set interrupt config to new sample ready"
	//// -- VL53L0X_SetGpioConfig() begin
//
	//WriteRegister(SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);
	//WriteRegister(GPIO_HV_MUX_ACTIVE_HIGH, ReadRegister(GPIO_HV_MUX_ACTIVE_HIGH) & ~0x10); // active low
	//WriteRegister(SYSTEM_INTERRUPT_CLEAR, 0x01);
//
	//// -- VL53L0X_SetGpioConfig() end
//
	//measurement_timing_budget_us = getMeasurementTimingBudget();
//
	//// "Disable MSRC and TCC by default"
	//// MSRC = Minimum Signal Rate Check
	//// TCC = Target CentreCheck
	//// -- VL53L0X_SetSequenceStepEnable() begin
//
	//WriteRegister(SYSTEM_SEQUENCE_CONFIG, 0xE8);
//
	//// -- VL53L0X_SetSequenceStepEnable() end
//
	//// "Recalculate timing budget"
	//setMeasurementTimingBudget(measurement_timing_budget_us);
//
	//// VL53L0X_StaticInit() end
//
	//// VL53L0X_PerformRefCalibration() begin (VL53L0X_perform_ref_calibration())
//
	//// -- VL53L0X_perform_vhv_calibration() begin
//
	//WriteRegister(SYSTEM_SEQUENCE_CONFIG, 0x01);
	//if (!performSingleRefCalibration(0x40)) { return false; }
//
	//// -- VL53L0X_perform_vhv_calibration() end
//
	//// -- VL53L0X_perform_phase_calibration() begin
//
	//WriteRegister(SYSTEM_SEQUENCE_CONFIG, 0x02);
	//if (!performSingleRefCalibration(0x00)) { return false; }
//
	//// -- VL53L0X_perform_phase_calibration() end
//
	//// "restore the previous Sequence Config"
	//WriteRegister(SYSTEM_SEQUENCE_CONFIG, 0xE8);
//
	//// VL53L0X_PerformRefCalibration() end

	return TRUE;
}





