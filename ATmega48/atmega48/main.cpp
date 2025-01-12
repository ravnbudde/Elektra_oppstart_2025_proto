/*
 * atmega48.cpp
 *
 * Created: 11.01.2025 15:27:31
 * Author : ravnr
 */ 


#include "include/vl53l0x.h"

volatile uint8_t flag = 0;


void timer1_init() {
	DDRB = (1<<PB1);
	
	TCCR1B |= (1<<CS11) | (1<<CS10);
	
	TIFR1 |= (1<<TOV1);
	TIMSK1 |= (1<<TOIE1);
	
}

ISR(TIMER1_OVF_vect){
	PORTB ^= (1<<PB1);
	flag = 0;
}


int main(void)
{
	I2C_Init(100000);
	
/*	timer1_init();*/
	
	sei();

	while (1)
	{
		I2C_WriteToAddress(0x29, 0x69);
		
		
/*
		while(flag);
		flag = 1;*/
	}
	
}


