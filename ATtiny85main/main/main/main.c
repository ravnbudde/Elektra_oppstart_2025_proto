/*
 * main.c
 *
 * Created: 25.11.2024
 * Author : eikel
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "include/I2C.h"

#define F_CPU 4000000UL 

volatile uint8_t brightness = 0;  // Duty cycle som ein prosentdel av OCR0A
volatile int8_t step = 1;         // Endringssteg for fading (+ eller -)

int main(void)
{
	// Sett PB0 som utgang
	DDRB |= (1 << DDB0);

	TCCR0A |= (1 << WGM00) | (1 << WGM01); // Fast PWM-modus
	TCCR0A |= (1 << COM0A1);               // Clear OC0A p� sammenligning, sett p� topp
	TCCR0B |= (1 << CS01);                 // Sett prescaler til 8 (4 MHz / 8 = 500 kHz PWM)

	// Aktiver interrupt p� Timer0 overflow
	TIMSK |= (1 << TOIE0);

	sei();

	while (1)
	{
		// Hovudl�kka gjer ingenting � 
	}
}

ISR(TIMER0_OVF_vect)
{
	brightness += step; // Endre lysstyrken

	// Sjekk for topp eller bunn av duty cycle
	if (brightness == 0 || brightness == 255)
	{
		step = -step; 
	}

	// Oppdater PWM duty cycle
	OCR0A = brightness;
}
