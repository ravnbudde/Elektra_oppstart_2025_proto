/*
 * Timer1.h
 *
 * Created: 26/11/2024 12:46:05
 *  Author: Ravn
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 8000000UL
#define TC1PS 1
#define OVFFREQ F_CPU/(255*TC1PS) //med instillinger over gir dette ~4ms tid, bør være mer enn nok

void init_timer1(void);
void delay_32us(void);


#endif /* TIMER1_H_ */