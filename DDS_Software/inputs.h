/*
 * inputs.h
 *
 *  Created on: Jan 28, 2014
 *      Author: Justin Flick
 */

#ifndef INPUTS_H_
#define INPUTS_H_
#include "main.h"

uint8_t joystickInit (uint8_t portno);
uint8_t joystick_read(void);
void updateParameters(userParam_t *userInput);






#endif /* INPUTS_H_ */
