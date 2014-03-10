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
	uint32_t  y;
	uint8_t recalcuateValues = 1;
//	userParam_t *templocation=userInput;

	/*	//template
	 *
	 *	if (userInput->___.changed==1){
	 *	x=userInput->___.currentValue;
	 *	y=(docalculation);  //based on relevant parameters
	 *	if(((y>userInput->___.max) userInput->___.currentValue=userInput->___.max;
	 *	else
	 *		if ((y<userInput->___.min)||(other condition)){
	 *		userInput->___.currentValue = userInput->___.min;
	 *		} else userInput->___.currentValue=y;
	 *		userInput->___.changed=0;
	 *	}
	 */
	while (recalcuateValues) {
		recalcuateValues = 0;

		if (userInput->Hz.changed == 1) {
			recalcuateValues = 1;
			y = 500000ul * (1 / userInput->Hz.currentValue);
			if (y > userInput->period.max) userInput->period.currentValue = userInput->period.max;
			else
				if ((y < userInput->period.min) || (y < 0)) {
					userInput->period.currentValue = userInput->period.min;
				} else userInput->period.currentValue = y;
			userInput->Hz.changed = 0;
		}

		if (userInput->period.changed == 1) {  //compute reciprocol and perform limit test
//			x = userInput->Hz.currentValue;
			y = 100000ul * (uint32_t) (1 / userInput->period.currentValue);
			if (y > userInput->Hz.max) userInput->Hz.currentValue = userInput->Hz.max;
			else
				if ((y < userInput->Hz.min) || (y < 0)) {
					userInput->Hz.currentValue = userInput->Hz.min;
				} else userInput->Hz.currentValue = y;
			userInput->Hz.changed = 0;
		}

		if (userInput->VPP.changed == 1) {
			recalcuateValues = 1;
//			userInput->vRMS=2;

		}

		if (userInput->vMax.changed == 1) {

		}

		if (userInput->vMin.changed == 1) {

		}

		if (userInput->vRMS.changed == 1) {

		}

		if (userInput->offset.changed == 1) {

		}
//	if (userInput->offset.changed == 1) {
//		x = userInput->offset.currentValue;
//		userInput->vMax.changed = userInput->vMin.changed = userInput->vRMS.changed=1;
//		userInput->VPP.changed = 1;
//
//		y = (docalculation);  //based on relevant parameters
//		if(((y>userInput->offset.max) userInput->offset.currentValue=userInput->period.max;
//						else
//						if ((y<userInput->offset.min)||(other condition)) {
//							userInput->offset.currentValue = userInput->offset.min;
//						} else userInput->offset.currentValue=y;
//						userInput->offset.changed = 0;
//					}

		/**
		 * Update the ad9833 parameters to match the user input
		 *
		 */

	}
}
