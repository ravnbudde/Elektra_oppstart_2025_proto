/*
 * main.c
 *
 * Created: 25.11.2024
 * Author : eikel
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 4000000UL // Set CPU-frekvensen til 1 MHz

volatile uint8_t brightness = 0;  // Duty cycle for PWM (0-1000)
volatile int8_t step = 1;         // Endringssteg for fading (+ eller -)

int main(void)
{
    // Sett PB1 (OC0B) som utgang
    DDRB |= (1 << DDB1);

    // Konfigurer Timer0 i Fast PWM, TOP = OCR0A (Mode 7)
    TCCR0A |= (1 << WGM00) | (1 << WGM01); // Fast PWM
    TCCR0A |= (1 << COM0B1);               // Clear OC0B on Compare Match
    TCCR0B |= (1 << WGM02);                // TOP = OCR0A
    TCCR0B |= (1 << CS02) | (1 << CS00);   // Prescaler = 1024

    // Sett toppverdi for 50 Hz frekvens
    OCR0A = 3; // for 50Hz: f_clk / (prescaler * PWM_freq) - 1 = 3906 / 50 - 1 = 77

    // Sett initial duty cycle
    OCR0B = 0;

    // Aktiver overflow interrupt
    TIMSK |= (1 << TOIE0);

    // Aktiver globale interrupt
    sei();

    while (1)
    {
        // Hovudløkka kan vere tom – fading skjer i interrupt
    }
}

ISR(TIMER0_OVF_vect)
{
	// Endre lysstyrken gradvis
	brightness += step;

	// Snu retning ved topp eller bunn
	if (brightness == 0 || brightness >= (255/OCR0A)*255)
	{
		step = -step;
	}
	// kan maks scale OCR0A*brightness = 255 => brightness = 255/OCR0A
	// Oppdater duty cycle
	OCR0B = (OCR0A * brightness) / 255; // Duty cycle i prosent av OCR0A
}
