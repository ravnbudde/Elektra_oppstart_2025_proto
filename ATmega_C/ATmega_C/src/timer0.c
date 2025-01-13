/*
 * timer0.c
 *
 * Created: 13.01.2025 13:33:01
 *  Author: ravnr
 */ 
#include "../inc/timer0.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t timer0_milliseconds = 0;

void Timer0_Init() {
	// Sett Timer0 til CTC-modus
	TCCR0A = (1 << WGM01); // Aktiver CTC-modus
	TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler = 64

	// Sett OCR0A for 1 ms intervall
	OCR0A = 125; // For 8 MHz og prescaler 64

	// Aktiver Compare Match A Interrupt
	TIMSK0 = (1 << OCIE0A);

	// Nullstill timer og tellervariabel
	TCNT0 = 0;
	timer0_milliseconds = 0;
}

void restart_millis() {
	timer0_milliseconds = 0; // Nullstill millisekund-teller
}

uint16_t millis() {
	return timer0_milliseconds;
}

// ISR for Timer0 Compare Match
ISR(TIMER0_COMPA_vect) {
	timer0_milliseconds++;
}
