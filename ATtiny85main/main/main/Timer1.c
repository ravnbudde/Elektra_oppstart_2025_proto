/*
 * Timer1.c
 *
 * Created: 26/11/2024 12:47:08
 *  Author: Ravn
 */ 

#include "include/Timer1.h"

void init_timer1(void){
	//Skru p� OVF interupt	
	TIFR |= (1<<TOV1);
	TIMSK |= (1<<TOIE1);
	
}

void delay_32us(void){
	TCNT1 = 0;
	TCCR1 |= (1<<CS10); //Skru p� klokke med 1 prescale
	
	while((TIFR & (1<<TOV1))); //Vent p� en ovf Litt usikker p� om denne faktisk fungerer?? m� sjekkes med oscilloskop
	TIFR |= (1<<TOV1); //Reset flag
	TCCR1 &= ~(1<<CS10); //Skru av klokka
	
	return;
}
