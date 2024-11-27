/*
 * main.c
 *
 * Created: 27.11.2024 17:18:36
 * Author : eikel
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "i2c.h"
#include "ds1307.h"

int main() {
	uint8_t hours, minutes, seconds;

	i2c_init();
	ds1307_init();

	// Sett initial tid
	ds1307_set_time(12, 30, 45);

	while (1) {
		ds1307_get_time(&hours, &minutes, &seconds);
		// Her kan du legge inn kode for å vise tid, f.eks. over UART
		_delay_ms(1000);
	}

	return 0;
}
