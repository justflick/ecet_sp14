/*
 * joystick.h
 *
 *  Created on: Jan 28, 2014
 *      Author: Konecranes
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_
#include "main.h"

//char  joystickInit (uint8_t portnon);

//uint8_t joystickInit(uint8_t portno);
//uint8_t joystickInit (uint8_t portnon);
uint8_t joystickInit (uint8_t portno);
uint8_t joystick_read(void);
volatile uint16_t msLast;





#endif /* JOYSTICK_H_ */
