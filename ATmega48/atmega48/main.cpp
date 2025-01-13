/*
 * atmega48.cpp
 *
 * Created: 11.01.2025 15:27:31
 * Author : ravnr
 */ 


#include "include/vl53l0x.h"



int main(void)
{
	DDRB |= (1<<PB1);
	cli();
	Timer0_Init();
	tw_init(TW_FREQ_100K, true);
	VL53L0X laser;
	laser.setTimeout(500);
	laser.init();
	laser.setMeasurementTimingBudget(20000);
	sei();

	while (1)
	{
		restart_millis();
		PORTB |= (1<<PB1);
		while(millis() < 200);
		restart_millis();
		PORTB &= (1<<PB1);
		laser.readRangeSingleMillimeters();
		while(millis() < 200);
			
		//time = millis();
		//while(time + 100 > millis());
		
		
		if(laser.timeoutOccurred()){
			PORTB |= (1<<PB1);
		}
	}
	
}


