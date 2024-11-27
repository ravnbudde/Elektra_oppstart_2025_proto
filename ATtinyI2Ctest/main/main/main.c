/*
 * main.c
 *
 * Created: 27.11.2024 17:18:36
 * Author : eikel
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_primary.h"
#include "ds1307.h"

#define LED_TOGGLE_DELAY_MS    500

void error_loop() {
	while (1) {
		PORTB |= (1 << PIN_LED); // Slå på LED
		_delay_ms(LED_TOGGLE_DELAY_MS);
		PORTB &= ~(1 << PIN_LED); // Slå av LED
		_delay_ms(LED_TOGGLE_DELAY_MS);
	}
}

int main() {
	// Initialisering
	i2c_init();
	ds1307_init();

	// Sett LED som utgang
	DDRB |= (1 << PIN_LED);
	PORTB &= ~(1 << PIN_LED); // LED av i starten

	// Testskriving til DS1307: sett klokken til 12:34:56, dato til søndag 25.12.2024
	ds1307_set_time(56, 34, 12);
	ds1307_set_date(7, 25, 12, 24);

	uint8_t seconds, minutes, hours;
	uint8_t day, date, month, year;

	while (1) {
		// Blink LED for aktivitet
		PORTB |= (1 << PIN_LED);
		_delay_ms(100);
		PORTB &= ~(1 << PIN_LED);

		// Kort forsinkelse
		_delay_ms(1000);
		// Les tid fra DS1307
		ds1307_get_time(&seconds, &minutes, &hours);
		
		PORTB |= (1 << PIN_LED);
		_delay_ms(100);
		PORTB &= ~(1 << PIN_LED);
		
		// Les dato fra DS1307
		ds1307_get_date(&day, &date, &month, &year);

		// Her kan du legge inn kode for å vise tid og dato
		// F.eks. via UART eller en OLED-skjerm
		// For debugging kan du også bruke LED for indikasjon

		// Kort forsinkelse
		_delay_ms(1000);
	}

	return 0;
}
