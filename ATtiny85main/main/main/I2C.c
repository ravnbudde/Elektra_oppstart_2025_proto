/*
 * I2C.c
 *
 * Created: 26/11/2024 12:33:56
 *  Author: Ravn
 */ 
#include "include/I2C.h"

/*
NB:
ca 63.75 us 'hold' på klokkesignalet. Det vil si bitrate frekvens: ~7.8kHz. Litt treigt, kan evt gjøres raskere med CTC mode om nødvendig
Men en melding + respons (8 addr, 2 ack, 8 data, 2 stop bit) er 20 bits så det holder til nesten 400 meldinger i sekundet
Bør derfor være mer enn nok
(1 melding med svar tar ca 2.5ms. Om LiDARen roterer 360 grader per sekund vil det si vi kan få unik måling for hver grad give or take)
Om vi skal bruke ATtinyen til noe mer enn en DAC burde hastigheten økes for å frigjøre blocking-tid
*/

void init_i2c_master(void){
	//Husk pullup-resistor på SDA og SCL linjene!!!
	//Side 112 i datasheet
	//Ser ut som den ikke har en i2c driver så må gjøre det litt manuelt

	USICR |= (1<< USIWM1) | (1<<USICS1) | (1 << USICLK); //Skru på 2-wire mode, tell klokke i software (USITC)
	DDRB |= (1<<DDB0) | (1<<DDB2); //Sett SCL og SDA som output
	PORTB |= (1<<SDA) | (1<<SCL); //Bus-sleep-state = høy, høy
}


void i2c_start_condition(void){
	PORTB &= ~(1<<SDA); //sett SDA lav
	delay_32us();
	PORTB &= ~(1<<SCL); //Sett SCL lav
	delay_32us();
}
void i2c_stop_condition(void){
	PORTB |= (1<<SCL); //sett SCL høy
	delay_32us();
	PORTB |= (1<<SDA); //sett SDA høy
	delay_32us();
}

uint8_t i2c_recieve_ack(void){
	//Bør legge til så den returnerer 0 om den leser en 0
	DDRB &= ~(1<<DDB0); //Sett SDA som input
	USISR |= (1<<USICNT3) | (1<<USICNT2) | (1<<USICNT1); //Sett counter til 1110 så ovf er etter 1 bit

	USICR |= (1<<USITC); //Skrur SCL høy og øker klokkecount
	while((PINB & (1<<PB2))); //Vent til SCL faktisk er høy (kan bli holdt lav av slave om klokken går for fort for den)
	delay_32us();
	USICR |= (1<<USITC); //Tror denne bare toggler klokka, så her vil den bli satt lav igjen
	delay_32us();
	return 1;
}

void i2c_send_byte(uint8_t data){
	
	
	USIDR = data; //Gjør klar data som skal sendes

	//Note: klokka teller 1 hver gang USITC blir satt til 1, altså teller klokka 2 per egentlige klokkesignal (teller edges), bruker derfor 16 increments (tror jeg med strek under tror)
	//Tror også ATtinyen automatisk sender neste bit i USIDR register når SCL blir satt lav da den er init som master
	while(!(USISR & (1<<USIOIF))){ //så før overflow (4bit => når klokka går fra 15 til 0)
		USICR |= (1<<USITC); //Skrur SCL høy og øker klokkecount	
		while((PINB & (1<<PB2))); //Vent til SCL faktisk er høy (kan bli holdt lav av slave om klokken går for fort for den)
		delay_32us();
		USICR |= (1<<USITC); //Tror denne bare toggler klokka, så her vil den bli satt lav igjen
		delay_32us();
	}
	USISR |= (1<<USIOIF); //Skru av overflow flagget
}

void i2c_write(uint8_t addr, uint8_t data){
	uint8_t acked = 0;
	i2c_start_condition();
	while(!(USISR & (1<<USISIF))); //Vent til start-condition flag blir satt (WaitTilBitIsCleared)
	USISR |= (1<<USISIF); //Clear flagget

	i2c_send_byte((addr<<1) | 0x00); // 0 for write
	acked = i2c_recieve_ack(); //Må fikse: lese fra USIBR for å se om den ble ack'ed eller ikke

	if(!acked){
		i2c_stop_condition();
		return;
	}

	i2c_send_byte(data); //kan også legge til funksjon for å sende flere bytes i en transfer
	acked = i2c_recieve_ack();

	i2c_stop_condition();
	return;
}

uint8_t i2c_read_byte(uint8_t send_ack) {
	uint8_t received_data;

	DDRB &= ~(1 << DDB0);  // Sett SDA som inngang
	USISR = 0xF0;          // Nullstill flag og teller (16 klokkesykluser for byte)

	while (!(USISR & (1 << USIOIF))) {  // Vent til alle bits er mottatt
		USICR |= (1 << USITC);          // Sett SCL høy
		while (!(PINB & (1 << PB2)));   // Vent til SCL faktisk er høy
		delay_32us();
		USICR |= (1 << USITC);          // Sett SCL lav
		delay_32us();
	}

	received_data = USIDR;  // Les innholdet i dataregisteret

	// Send ACK eller NACK
	DDRB |= (1 << DDB0);                // Sett SDA som utgang
	if (send_ack) {
		PORTB &= ~(1 << PB0);           // Send ACK (lav)
		} else {
		PORTB |= (1 << PB0);            // Send NACK (høy)
	}
	USISR = (1 << USICNT3) | (1 << USICNT2) | (1 << USICNT1);  // 1 klokkesyklus for ACK/NACK
	USICR |= (1 << USITC);              // Sett SCL høy
	while (!(PINB & (1 << PB2)));       // Vent til SCL faktisk er høy
	delay_32us();
	USICR |= (1 << USITC);              // Sett SCL lav
	delay_32us();

	return received_data;
}

void i2c_read(uint8_t addr, uint8_t *buffer, uint8_t length){
	uint8_t acked = 0;
	i2c_start_condition();
	while(!(USISR & (1<<USISIF))); //Vent til start-condition flag blir satt (WaitTilBitIsCleared)
	USISR |= (1<<USISIF); //Clear flagget

	i2c_send_byte((addr<<1) | 0x01); //Legg til 1 i slutten for å sende read
	acked = i2c_recieve_ack(); //Må fikse: lese fra USIBR for å se om den ble ack'ed eller ikke

	if(!acked){
		i2c_stop_condition();
		return;
	}

	for(int i = 0; i < length; i++){
		if(i < length - 1){
			buffer[i] = i2c_read_byte(1); //send ACK (forventer fler bytes)
		}
		else if(i == length - 1){
			buffer[i] = i2c_read_byte(0); //send NACK (ferdig)
		}
	}
	i2c_stop_condition();
	return;
}


