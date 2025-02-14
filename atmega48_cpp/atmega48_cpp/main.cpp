/*
 * atmega48_cpp.cpp
 *
 * Created: 15.01.2025 17:34:44
 * Author : ravnr
 */ 


#include "vl53l0x.h"



int main(void)
{
	
/*	timer1_init();*/
	
	DDRB |= (1<<PB1);
	cli();
	Timer0_Init();
	VL53L0X laser;
	laser.setTimeout(500);
	laser.init();
	laser.setMeasurementTimingBudget(20000);
	sei();
	uint8_t time = 0;

	while (1)
	{
		restart_millis();
		PORTB |= (1<<PB1);
		while(millis() < 200);
		PORTB &= (1<<PB1);
		laser.readRangeSingleMillimeters();		
		//time = millis();
		//while(time + 100 > millis());
		
		
/*
		while(flag);
		flag = 1;*/
		if(laser.timeoutOccurred()){
			PORTB &= ~(1<<PB1);
		}
	}
	
}


