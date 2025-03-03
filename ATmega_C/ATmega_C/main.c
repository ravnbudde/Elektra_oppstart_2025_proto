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

	DDRB |= (1<<PB1);


	cli();
	Timer0_Init();
	tw_init(TW_FREQ_100K, TRUE); //Bør være true tror jeg
	//VL53L0X_init(device);
	sei();
	
    while (1) 
    {
		//tw_start();
		//tw_write((0x29<<1)|1);
		//tw_stop();
		
		restart_millis();
		while (millis() < 100);
		PORTB ^= (1<<PB1);
		//VL53L0X_read_mm(device);
    }
}

