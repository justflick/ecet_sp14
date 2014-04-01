/*
 * joystick.c
 *
 *  Created on: Jan 19, 2014
 *      Author: Justn Flick
 */

#include "main.h"

/**
 * @brief Initialize the analog interface to free-running mode
 * @param portno: specift the ADC channel activate
 *
 * todo: Ensure that proper ADC channel is set to input mode.
 */
void joystickInit(uint8_t portno) {
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	//enable AD peripheral

	ADCSRA |= (0x4 << ADPS0);  //set prescale to F_CPU/64
	ADMUX |= (1 << REFS0) | portno;  //set Vref to AVCC, set port to parameter val
	ADCSRA |= (1 << ADATE);  //Set AD to auto-trigger
	ADMUX |= (1 << ADLAR);	//set AD adjustment left mode for 8bit read
	ADCSRA |= (1 << ADIE);	//enable AD interupt
	ADCSRA |= (1 << ADSC);	//Start first conversion

}
/**
 *
 * @param ADC_vect takes no parameters.
 * @brief The ADC interrupt is being used to sample the
 * analog input and create an average value based on 16
 * datapints. The purpose of this is to avoid sampling
 * a transitory value as an incorrect keypress.
 *
 * The ISR takes no parameters and updates a global volatile
 * variable.
 */
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
/**
 *
 * @return returns a character which is mapped to keypress definitions.
 * @brief Function takes the analog value from the
 * button port and makes a decision to determine which keypress to
 * return. Keymappings are provided in main.h and are based off
 * of the tactile switch network using 2kohm resistors and the same Vref
 * as the AVR controller.
 */
uint8_t joystick_read(void) {
	uint8_t buttonVal = JOYSTICK_NOPRESS, buttonTemp = pbVal;
	uint16_t pressedTime = systemTicks;

	if (buttonTemp < 20) {
		buttonVal = JOYSTICK_NOPRESS;
	}

	_delay_us(100);
	buttonTemp = pbVal;

	if ((200 < buttonTemp) && (buttonTemp < 230)) buttonVal = JOYSTICK_DOWN;
	else
		if ((150 < buttonTemp) && (buttonTemp < 180)) buttonVal = JOYSTICK_RIGHT;
		else
			if ((110 < buttonTemp) && (buttonTemp < 130)) buttonVal = JOYSTICK_ENTER;
			else
				if ((70 < buttonTemp) && (buttonTemp < 90)) buttonVal = JOYSTICK_LEFT;
				else
					if ((30 < buttonTemp) && (buttonTemp < 50)) buttonVal = JOYSTICK_UP;

	while (pbVal > 20) {
		if (pressedTime + 500 < systemTicks) break;  //debounce and allow for repeat.
	}
	delayTicker_ms(30);
	return buttonVal;
}
/**
 * @briefFunction steps through the nested structs to find values which require
 * updating. For instance, a change in frequency will be reflected in
 * the period.
 * @param userInput takesaddress to struct of type userParam_t
 */
void updateParameters(parameter_defs *arg) {
	uint8_t i;
	uint32_t y;

	if (&arg->currentValue == &userParameters.Hz.currentValue) {
		ad9833_set_frequency(arg->currentValue / 100);	//test value
//		y = 500000ul * (1 / arg->currentValue);
//		if (y > arg->max) arg->currentValue = arg->max;
//		else
//			if ((y < arg->min) || (y < 0)) {
//				arg->currentValue = arg->min;
//			} else arg->currentValue = y;
	}
	if (&arg->currentValue == &userParameters.period.currentValue) {  //compute reciprocol and perform limit test

		y = 100000ul * (uint32_t) (1 / arg->currentValue);
		if (y > arg->max) arg->currentValue = arg->max;
		else
			if ((y < arg->min) || (y < 0)) {
				arg->currentValue = arg->min;
			} else arg->currentValue = y;
	}
	if (&arg->currentValue == &userParameters.offset.currentValue) {
	}
	if (&arg->currentValue == &userParameters.vMax.currentValue) {
	}
	if (&arg->currentValue == &userParameters.vMin.currentValue) {
	}
	if (&arg->currentValue == &userParameters.VPP.currentValue) {
		i = (uint8_t) arg->currentValue;
		ad5204SetVal(i, 0);
	}
	if (&arg->currentValue == &userParameters.vRMS.currentValue) {
	}
	if (&arg->currentValue == &userParameters.PWM.currentValue) {

	}
	if (&arg->currentValue == &userParameters.phase.currentValue) {
		ad9833_set_phase(arg->currentValue/100, 0, 1);
	}
	if (&arg->currentValue == &userParameters.dutyCycle.currentValue) {

	}
}

