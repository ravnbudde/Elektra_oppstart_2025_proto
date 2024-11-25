/*
 * main.c
 *
 * Created: 25.11.2024 16:27:00
 * Author : eikel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 1000000UL // Set CPU-frekvensen til 1 MHz

volatile uint8_t brightness = 0;  // Duty cycle for PWM
volatile int8_t step = 1;         // Endringssteg for fading (+ eller -)

int main(void)
{
	cli();
	DDRB |= (1 << DDB0);
	// Sett opp Timer0 i CTC-modus, Clear OC0B på sammenligning
	TCCR0A |= (1 << WGM01) | (1 << COM0B1);   
	      
	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescaler = 1024  
	
	// Sett toppverdi for 50 Hz PWM
	OCR0A = 77; //f_clk/(prescale*PWM_Hz) - 1 = 3906 / 50 - 1 = 77
	OCR0B = 0;
	// Aktiver interrupt på Timer0 overflow
	TIMSK |= (1 << TOIE0);
	sei();

	while (1)
	{
		// Hovudløkka gjer shit all
	}
}

ISR(TIMER0_COMPA_vect){
	brightness += step;

	// Sjekk for topp eller bunn av duty cycle
	if (brightness == 0 || brightness >= OCR0A)
	{
		step = -step;
	}

	// Oppdater PWM duty cycle
	OCR0B = brightness;
}

