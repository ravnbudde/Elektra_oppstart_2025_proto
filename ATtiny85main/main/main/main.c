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

#define F_CPU 8000000UL 

volatile uint8_t brightness = 0;  // Duty cycle som ein prosentdel av OCR0A
volatile int8_t step = 1;         // Endringssteg for fading (+ eller -)

// legg til  -Ulfuse:w:0xE2:m i deply for å fuse intern klokke 8MHz
int main(void)
{
	///*Skru av watchdogTimer*/
	///* Clear WDRF in MCUSR */
	//MCUSR = 0x00;
	///* Write logical one to WDCE and WDE */
	//WDTCR |= (1<<WDCE) | (1<<WDE);
	///* Turn off WDT */
	//WDTCR = 0x00;
	
	
	// Sett PB1 som utgang
	DDRB |= (1 << DDB1);

	TCCR0A |= (1 << WGM00) | (1 << WGM01); // Fast PWM-modus
	TCCR0A |= (1 << COM0B1);               // Clear OC0A på sammenligning, sett på topp
	TCCR0B |= (1 << CS01) | (1<<CS00);  
	//OCR0B = 250;              

	// Aktiver interrupt på Timer0 overflow
	
	TIMSK |= (1 << TOIE0);
	
	
	//eks på I2C sending. 
	init_timer1();
	init_i2c_master();
	uint8_t address = 69;
	uint8_t buffer[2];
	
	sei();
	
	
	i2c_read(0x29, buffer, 2);
	
	

	while (1)
	{
		// Hovudløkka gjer ingenting – 		
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
	OCR0B = brightness;
}
