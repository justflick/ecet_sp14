/*
 * joystick.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Justn Flick
 */

#include "main.h"

/**
 * @brief Initialize the analog interface in free-running mode
 * @param portno: specift the ADC channel to be made active
 * @return return success when done
 *
 * todo: Ensure that proper ADC channel is set to input mode.
 */
uint8_t joystickInit(uint8_t portno) {
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	//enable AD peripheral

	ADCSRA |= (0x4 << ADPS0);	//set prescale to F_CPU/64
	ADMUX |= (1 << REFS0) | portno; //set Vref to AVCC, set port to parameter val
	ADCSRA |= (1 << ADATE);	//Set AD to auto-trigger
	ADMUX |= (1 << ADLAR);	//set AD adjustment left mode for 8bit read
	ADCSRA |= (1 << ADIE);	//enable AD interupt
	ADCSRA |= (1 << ADSC);	//Start first conversion

	return 0;
}

ISR(ADC_vect) {
	static uint16_t sum;
	static uint8_t i = 0;

	if (i == 16) {
		pbVal = sum / 16;
		i = 0;
		sum = 0;
	} else {
		sum += ADCH;
		i++;
	}
}

uint8_t joystick_read(void) {
	uint8_t buttonVal = JOYSTICK_NOPRESS, buttonTemp = pbVal;
	uint16_t pressedTime = systemTicks;
//	volatile uint16_t msLast;
//	serialWriteString("\nButton read=");
//	if (msLast + 80 < systemTicks) {
//		 ;		//debounce counter

	if (buttonTemp < 20) {
//		serialWriteString("\nnopress");
		buttonVal= JOYSTICK_NOPRESS;
	} else if ((200 < buttonTemp) && (buttonTemp < 230)) {
		serialWriteString("\ndown");
		buttonVal = JOYSTICK_DOWN;
	} else if ((150 < buttonTemp) && (buttonTemp < 180)) {
		serialWriteString("\nright");
		buttonVal = JOYSTICK_RIGHT;
	} else if ((110 < buttonTemp) && (buttonTemp < 130)) {
		serialWriteString("\neneter");
		buttonVal = JOYSTICK_ENTER;
	} else if ((70 < buttonTemp) && (buttonTemp < 90)) {
		serialWriteString("\nleft");
		buttonVal = JOYSTICK_LEFT;
	} else if ((30 < buttonTemp) && (buttonTemp < 50)) {
		serialWriteString("\nup");
		buttonVal = JOYSTICK_UP;
	}
	delayTicker_ms(2);
	while (pbVal>20){
		if (pressedTime+500<systemTicks) break;	//debounce and allow for repeat.
	}
//	} else
//		return JOYSTICK_NOPRESS;
//	msLast = systemTicks;
//	serialWriteNum(pbVal, 1);
//	serialWriteString('0' + buttonVal);
	delayTicker_ms(20);

	return buttonVal;
}
/**
 * @briefFunction steps through the nested structs to find values which require
 * updating. For instance, a change in frequency will be reflected in
 * the period.
 * @param userInput takesaddress to struct of type userParam_t
 */
void updateParameters(userParam_t *userInput) {
	uint32_t y;
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
			if (y > userInput->period.max)
				userInput->period.currentValue = userInput->period.max;
			else if ((y < userInput->period.min) || (y < 0)) {
				userInput->period.currentValue = userInput->period.min;
			} else
				userInput->period.currentValue = y;
			userInput->Hz.changed = 0;
		}

		if (userInput->period.changed == 1) {  //compute reciprocol and perform limit test
//			x = userInput->Hz.currentValue;
			y = 100000ul * (uint32_t) (1 / userInput->period.currentValue);
			if (y > userInput->Hz.max)
				userInput->Hz.currentValue = userInput->Hz.max;
			else if ((y < userInput->Hz.min) || (y < 0)) {
				userInput->Hz.currentValue = userInput->Hz.min;
			} else
				userInput->Hz.currentValue = y;
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

//		ad9833_set_frequency()
		/**
		 * Update the ad9833 and ad5204 parameters to match the user input
		 *
		 */

	}
}
