/*
 * ATmega_C.c
 *
 * Created: 13.01.2025 13:31:03
 * Author : ravnr
 */ 
#include "inc/vl53l0x.h"


int main(void)
{
	struct VL53L0X* device;
	device->address = 0x29;
	device->last_status = 0;
	device->stop_variable = 0;
	device->last_measure_mm = 0;

	cli();
	Timer0_Init();
	tw_init(TW_FREQ_100K, TRUE);
	VL53L0X_init(device);
	sei();
	
    while (1) 
    {
		restart_millis();
		while (millis() < 100);
		
		VL53L0X_read_mm(device);
    }
}

