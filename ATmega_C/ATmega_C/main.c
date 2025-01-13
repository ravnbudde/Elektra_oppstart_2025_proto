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

	tw_init(TW_FREQ_100K, TRUE);
	VL53L0X_init(device);

    while (1) 
    {
    }
}

