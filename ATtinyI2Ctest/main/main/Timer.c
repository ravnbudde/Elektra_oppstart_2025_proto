/*
 * Timer.c
 *
 * Created: 27.11.2024 17:42:18
 *  Author: eikel
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

void timer0_init(void) {
	// Sett opp Timer0 i CTC-modus
	TCCR0A = (1 << WGM01);
	// Sett prescaler til 1024
	TCCR0B = (1 << CS02) | (1 << CS00);
	// Sett sammenligningsverdi for 1 sekund (forutsatt 8 MHz klokke)
	OCR0A = 15624;
	// Aktiver Output Compare Match A Interrupt
	TIMSK = (1 << OCIE0A);
	// Aktiver globale avbrudd
	sei();
}