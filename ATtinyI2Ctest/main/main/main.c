/*
 * main.c
 *
 * Created: 27.11.2024 17:18:36
 * Author : eikel
 */ 



/*
 * main.c
 *
 * Created: 11/27/2024 8:15:49 PM
 *  Author: ravneb
 */ 

#include <xc.h>
#include <avr/io.h>
#include <stdio.h>
#define F_CPU 8000000UL 
#include <util/delay.h>

#include "i2c_primary.h"
#include "ds1307.h"

#define PIN_LED 					PB1

#define LASER_ADDRESS		 		0x09

volatile uint8_t data = 0;

void error_loop() {
	while(1) { // inifinte loop
		PORTB |= (1 << PIN_LED);
		_delay_ms(250);
		PORTB &= ~(1 << PIN_LED);
		_delay_ms(250);
	}
}

int main() {
	
	// Sett PB1 som utgang
	DDRB |= (1 << DDB1);

	TCCR0A |= (1 << WGM00) | (1 << WGM01); // Fast PWM-modus
	TCCR0A |= (1 << COM0B1);               // Clear OC0A p� sammenligning, sett p� topp
	TCCR0B |= (1 << CS01) | (1<<CS00);
	//OCR0B = 250;

	// Aktiver interrupt p� Timer0 overflow
		
	TIMSK |= (1 << TOIE0);
	

	
	_delay_ms(1500); // some min init

	i2c_init();
	
	sei();

	unsigned char read_byte = 0x00;
	unsigned int read_word = 0x00;
	

	while(1) {
			

		/*Funker?*/
		// READ to get the button state
		read_byte = 0x00;
		read_word = 0x00;
		i2c_start();
		// BUTTON DEVICE (0xBA for write, 0xBB for read)
		if (!i2c_write_address(LASER_ADDRESS, READ)) { // read address
			//error_loop();
			data += 10; 
			i2c_read_byte(0);
		}
	
		i2c_stop();
		
		
		
		
		_delay_ms(500);
	}


}


ISR(TIMER0_OVF_vect)
{
	// Oppdater PWM duty cycle
	OCR0B = data;
}













 /*!!!!!!!!!!!!!!!!!! */

// #include <avr/io.h>
// #include <util/delay.h>
// #include "i2c_primary.h"
// #include "ds1307.h"

// #define LED_TOGGLE_DELAY_MS    500

// void error_loop() {
// 	while (1) {
// 		PORTB |= (1 << PIN_LED); // Sl� p� LED
// 		_delay_ms(LED_TOGGLE_DELAY_MS);
// 		PORTB &= ~(1 << PIN_LED); // Sl� av LED
// 		_delay_ms(LED_TOGGLE_DELAY_MS);
// 	}
// }

// int main() {
// 	// Initialisering
// 	i2c_init();
// 	ds1307_init();

// 	// Sett LED som utgang
// 	DDRB |= (1 << PIN_LED);
// 	PORTB &= ~(1 << PIN_LED); // LED av i starten

// 	// Testskriving til DS1307: sett klokken til 12:34:56, dato til s�ndag 25.12.2024
// 	ds1307_set_time(56, 34, 12);
// 	ds1307_set_date(7, 25, 12, 24);

// 	uint8_t seconds, minutes, hours;
// 	uint8_t day, date, month, year;

// 	while (1) {
// 		// Blink LED for aktivitet
// 		PORTB |= (1 << PIN_LED);
// 		_delay_ms(100);
// 		PORTB &= ~(1 << PIN_LED);

// 		// Kort forsinkelse
// 		_delay_ms(1000);
// 		// Les tid fra DS1307
// 		ds1307_get_time(&seconds, &minutes, &hours);
		
// 		PORTB |= (1 << PIN_LED);
// 		_delay_ms(100);
// 		PORTB &= ~(1 << PIN_LED);
		
// 		// Les dato fra DS1307
// 		ds1307_get_date(&day, &date, &month, &year);

// 		// Her kan du legge inn kode for � vise tid og dato
// 		// F.eks. via UART eller en OLED-skjerm
// 		// For debugging kan du ogs� bruke LED for indikasjon

// 		// Kort forsinkelse
// 		_delay_ms(1000);
// 	}

// 	return 0;
// }

/*
#include <Wire.h>

// Globale variabler
volatile uint8_t sequenceNumber = 0; // Sekvensnummer som skal sendes tilbake

void setup() {
	// Start I2C som slave med adresse 0x08
	Wire.begin(0x08);

	// Konfigurer callback-funksjoner
	Wire.onReceive(receiveEvent); // Kalles n�r data mottas fra master
	Wire.onRequest(requestEvent); // Kalles n�r master ber om data

	// Start Serial Monitor for debugging
	Serial.begin(9600);
	while (!Serial) {
		; // Vent til Serial er klar
	}

	Serial.println("Arduino Due er klar som I2C-slave p� adresse 0x08.");
}

void loop() {
	// Hovedl�kken gj�r ingenting; alt h�ndteres i interrupt-callbacks
	delay(100);
}

// Callback n�r data mottas fra master
void receiveEvent(int numBytes) {
	// Les innkommende data fra master
	while (Wire.available()) {
		char receivedByte = Wire.read();
		Serial.print("Mottok data fra master: ");
		Serial.println(receivedByte);
	}
}

// Callback n�r master ber om data
void requestEvent() {
	// Send sekvensnummer til master
	Wire.write(sequenceNumber);

	// Debug info til Serial Monitor
	Serial.print("Sendte sekvensnummer: ");
	Serial.println(sequenceNumber);

	// Oppdater sekvensnummer for neste foresp�rsel
	sequenceNumber++;
}
*/