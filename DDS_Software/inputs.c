/*
 * joystick.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Justn Flick
 */

#include "main.h"
//#include "timer.h"

/**
 * @brief Initialize the analog interface in free-running mode
 * @param portno: specift the ADC channel to be made active
 * @return return success when done
 *
 * todo: Ensure that proper ADC channel is set to input mode.
 */
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
	volatile uint16_t msLast;

	if (msLast + 80 < systemTicks) {
		buttonTemp = ADCH;		//debounce counter

		if (buttonTemp > 240) buttonVal = JOYSTICK_NOPRESS;
		if ((200 > buttonTemp) && (buttonTemp > 180)) buttonVal = JOYSTICK_UP;
		if ((160 > buttonTemp) && (buttonTemp > 140)) buttonVal = JOYSTICK_DOWN;
		if ((120 > buttonTemp) && (buttonTemp > 100)) buttonVal = JOYSTICK_LEFT;
		if ((80 > buttonTemp) && (buttonTemp > 60)) buttonVal = JOYSTICK_RIGHT;
		if ((40 > buttonTemp) && (buttonTemp > 20)) buttonVal = JOYSTICK_ENTER;
	} else return JOYSTICK_NOPRESS;
	msLast = systemTicks;

	return buttonVal;
}
/**
 * @briefFunction steps through the nested structs to find values which require
 * updating. For instance, a change in frequency will be reflected in
 * the period.
 * @param userInput takesaddress to struct of type userParam_t
 */
void updateParameters(userParam_t *userInput) {
	uint16_t i, j, k;
	uint32_t x, y, z;

	if (userInput->Hz.changed == 1) {
		x = userInput->period.value;
		y = 500000ul * (1 / userInput->Hz.value);
		if (y > userInput->period.max) userInput->period.value = userInput->period.max;
		else
			if ((y < userInput->period.min) || (y < 0)) {
				userInput->period.value = userInput->period.min;
			} else userInput->period.value = y;
		userInput->Hz.changed = 0;
	}

	if (userInput->period.changed == 1) {
			x = userInput->Hz.value;
			y = 500000ul * (1 / userInput->period.value);
			if (y > userInput->Hz.max) userInput->Hz.value = userInput->Hz.max;
			else
				if ((y < userInput->Hz.min) || (y < 0)) {
					userInput->Hz.value = userInput->Hz.min;
				} else userInput->Hz.value = y;
			userInput->Hz.changed = 0;
		}

}
