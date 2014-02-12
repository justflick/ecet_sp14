/*
 * joystick.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Justn Flick
 */

#include "main.h"
//#include "timer.h"

uint8_t joystickInit(uint8_t portno) {

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADATE);
	ADMUX |= (1 << ADLAR);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADSC);


	uint8_t success = 0;
	return success;

}

uint8_t joystick_read(void) {
	uint8_t buttonVal = JOYSTICK_NOPRESS, buttonTemp = 0;

	if (msLast + 80 < ticks) {
		buttonTemp = ADCH;		//debounce counter

		if (buttonTemp > 240) buttonVal = JOYSTICK_NOPRESS;
		if ((200 > buttonTemp) && (buttonTemp > 180)) buttonVal = JOYSTICK_UP;
		if ((160 > buttonTemp) && (buttonTemp > 140)) buttonVal = JOYSTICK_DOWN;
		if ((120 > buttonTemp) && (buttonTemp > 100)) buttonVal = JOYSTICK_LEFT;
		if ((80 > buttonTemp) && (buttonTemp > 60)) buttonVal = JOYSTICK_RIGHT;
		if ((40 > buttonTemp) && (buttonTemp > 20)) buttonVal = JOYSTICK_ENTER;
	} else return JOYSTICK_NOPRESS;
	msLast = ticks;

	return buttonVal;

}

