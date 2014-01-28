/*
 * timer.h
 *
 *  Created on: Jan 28, 2014
 *      Author: Konecranes
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "main.h"

uint8_t timerInit(void){
	uint8_t status=0;
//	neat tool http://files.b9.com/avrcalc/

	cli();	//clear all interrupt bits
	TCCR1A=0;
	TCCR1B=0;

	TIMSK1=(1<<TOIE1);
	TCCR1B|=(1<<CS10);

	sei();
	OCR0A=0x0270;
	TCNT0=0xfd8f;


	//init timer code to 1ms ticks

	return status
}

is




#endif /* TIMER_H_ */
