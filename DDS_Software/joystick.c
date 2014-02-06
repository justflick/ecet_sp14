/*
 * joystick.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Justn Flick
 */

#include "main.h"
#include "timer.h"

uint8_t joystickInit(uint8_t portno) {

	DDRB |= (1 << PB5);    //PB5/digital 13 is an output

	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));  //Prescaler at 128 so we have an 125Khz clock source
	ADMUX |= (1 << REFS0);
	ADMUX &= (0 << REFS1);                //Avcc(+5v) as voltage reference
	ADCSRB &= ((0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0));    //ADC in free-running mode
	ADCSRA |= (1 << ADATE);                //set to free-run mode
	ADCSRA |= (1 << ADEN);                //ADC enable power
	ADCSRA |= (1 << ADSC);                //ADC begin conversion

//	initialize to
//	secified port
	uint8_t success = 0;
	return success;

}

uint8_t joystick_read(uint8_t usec) {
	uint8_t buttonVal = JOYSTICK_NOPRESS, buttonTemp = 0;
	uint16_t msLast, msCurrent;

	msCurrent=ticks;
	if (msLast + 25 < msCurrent) {
		msLast=ticks;
		buttonTemp = ADCH;		//debounce counter

		if (buttonTemp > 240) buttonVal = JOYSTICK_NOPRESS;
		if ((200 > buttonTemp) && (buttonTemp > 180)) buttonVal = JOYSTICK_UP;
		if ((160 > buttonTemp) && (buttonTemp > 140)) buttonVal = JOYSTICK_DOWN;
		if ((120 > buttonTemp) && (buttonTemp > 100)) buttonVal = JOYSTICK_LEFT;
		if ((80 > buttonTemp) && (buttonTemp > 60)) buttonVal = JOYSTICK_RIGHT;
		if ((40 > buttonTemp) && (buttonTemp > 20)) buttonVal = JOYSTICK_ENTER;
	} else return JOYSTICK_NOPRESS;

	return buttonVal;

}

