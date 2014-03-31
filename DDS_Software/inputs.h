/*
 * inputs.h
 *
 *  Created on: Jan 28, 2014
 *      Author: Justin Flick
 */

#ifndef INPUTS_H_
#define INPUTS_H_
#include "main.h"

void joystickInit (uint8_t portno);
uint8_t joystick_read(void);
void updateParameters(parameter_defs *arg);
uint8_t volatile pbVal;






#endif /* INPUTS_H_ */
