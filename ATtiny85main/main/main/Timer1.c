/*
 * Timer1.c
 *
 * Created: 26/11/2024 12:47:08
 *  Author: Ravn
 */ 

#include "include/Timer1.h"

void init_timer1(void){
	//Skru på OVF interupt	
	TIFR |= (1<<TOV1);
	TIMSK |= (1<<TOIE1);
	
}

void delay_4ms(void){
	TCNT1 = 0;
	TCCR1 |= (1<<CS12) | (1<<CS11) | (1<<CS10); //Skru på klokke med 64 prescale
	
	while(!(TIFR & (1<<TOV1))); //Vent på en ovf
	
	//Skru av klokka
	TCCR1 &= ~(1<<CS12);
	TCCR1 &= ~(1<<CS11);
	TCCR1 &= ~(1<<CS10);
	
	return;
}
