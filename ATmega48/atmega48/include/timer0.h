/*
 * timer0.h
 *
 * Created: 12.01.2025 15:34:47
 *  Author: ravnr
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_
#include <avr/io.h>
#include <avr/interrupt.h>

void Timer0_Init();
void restart_millis();
uint16_t millis();


#endif /* TIMER0_H_ */
