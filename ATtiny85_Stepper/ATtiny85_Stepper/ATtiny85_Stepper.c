/*
 * ATtiny85_Stepper.c
 *
 * Created: 23.12.2024 18:58:59
 * Author : eikel
 */ 
#include <avr/io.h>
#include <util/delay.h>

// Definer motorpinnar
#define MOTOR_PIN1 PB0
#define MOTOR_PIN2 PB1
#define MOTOR_PIN3 PB2
#define MOTOR_PIN4 PB4

// Stegsekvens for fullstegskjøring
const uint8_t stepSequence[4] = {
	(1 << MOTOR_PIN1),
	(1 << MOTOR_PIN2),
	(1 << MOTOR_PIN3),
	(1 << MOTOR_PIN4)
};

void setup() {
	// Sett motorpinnar som utgang
	DDRB |= (1 << MOTOR_PIN1) | (1 << MOTOR_PIN2) | (1 << MOTOR_PIN3) | (1 << MOTOR_PIN4);
}

void loop() {
	while (1) {
		// Iterer gjennom stegsekvensen for å oppnå rotasjon
		for (uint8_t i = 0; i < 4; i++) {
			PORTB = (PORTB & 0xE0) | stepSequence[i]; // Oppdater motorpinnar
			_delay_ms(20); // Juster dinna verdien for å endre rotasjonshastigheit, 20 funka 10 funka ikkje
		}
	}
}

int main(void) {
	setup();
	loop();
	return 0;
}


/*MED ControllPIN*/
/*
 * ATtiny85_Stepper.c
 *
 * Created: 23.12.2024 18:58:59
 * Author : eikel
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Definer motorpinnar
#define MOTOR_PIN1 PB0
#define MOTOR_PIN2 PB1
#define MOTOR_PIN3 PB3
#define MOTOR_PIN4 PB4
#define CONTROL_PIN PB2 // Pin for å motta kontrollsignal (INT0)

// Stegsekvens for fullstegskjøring
const uint8_t stepSequence[4] = {
	(1 << MOTOR_PIN1),
	(1 << MOTOR_PIN2),
	(1 << MOTOR_PIN3),
	(1 << MOTOR_PIN4)
};

volatile uint8_t motorRunning = 0; // Variabel for å halde styr på motorstatus

void setup() {
	// Sett motorpinnar som utgang
	DDRB |= (1 << MOTOR_PIN1) | (1 << MOTOR_PIN2) | (1 << MOTOR_PIN3) | (1 << MOTOR_PIN4);
	// Sett kontrollpinnen som inngang med intern pull-up
	DDRB &= ~(1 << CONTROL_PIN);
	PORTB |= (1 << CONTROL_PIN);

	// Konfigurer INT0 for stigande flanke
	MCUCR |= (1 << ISC01) | (1 << ISC00); // Sett ISC01 og ISC00 for stigande flanke
	GIMSK |= (1 << INT0); // Aktiver ekstern avbrudd INT0

	sei(); // Aktiver globale avbrudd
}

void loop() {
	if (motorRunning) {
		// Iterer gjennom stegsekvensen for å oppnå rotasjon
		for (uint8_t i = 0; i < 4; i++) {
			PORTB = (PORTB & 0xE0) | stepSequence[i]; // Oppdater motorpinnar
			_delay_ms(20); // Juster denne verdien for å endre rotasjonshastigheit
		}
		} else {
		// Sett alle motorpinnar låge for å stoppe motoren
		PORTB &= 0xE0;
	}
}

// Avbrotsrutine for INT0
ISR(INT0_vect) {
	// Toggle motorstatus
	motorRunning ^= motorRunning;
}


int main(void) {
	setup();
	while (1) {
		loop();
	}
	return 0;
}



