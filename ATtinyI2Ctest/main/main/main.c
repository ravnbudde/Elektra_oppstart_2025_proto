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

/*
#include <Wire.h>

// Globale variabler
volatile uint8_t sequenceNumber = 0; // Sekvensnummer som skal sendes tilbake

void setup() {
	// Start I2C som slave med adresse 0x08
	Wire.begin(0x08);

	// Konfigurer callback-funksjoner
	Wire.onReceive(receiveEvent); // Kalles når data mottas fra master
	Wire.onRequest(requestEvent); // Kalles når master ber om data

	// Start Serial Monitor for debugging
	Serial.begin(9600);
	while (!Serial) {
		; // Vent til Serial er klar
	}

	Serial.println("Arduino Due er klar som I2C-slave på adresse 0x08.");
}

void loop() {
	// Hovedløkken gjør ingenting; alt håndteres i interrupt-callbacks
	delay(100);
}

// Callback når data mottas fra master
void receiveEvent(int numBytes) {
	// Les innkommende data fra master
	while (Wire.available()) {
		char receivedByte = Wire.read();
		Serial.print("Mottok data fra master: ");
		Serial.println(receivedByte);
	}
}

// Callback når master ber om data
void requestEvent() {
	// Send sekvensnummer til master
	Wire.write(sequenceNumber);

	// Debug info til Serial Monitor
	Serial.print("Sendte sekvensnummer: ");
	Serial.println(sequenceNumber);

	// Oppdater sekvensnummer for neste forespørsel
	sequenceNumber++;
}
*/