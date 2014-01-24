
/*
 * project.h
 *
 *  Created on: Feb 17, 2013
 *      Author: justin
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#define pbPin 5         //pin to be used for the analog button input
#define none 0
#define up 1
#define down 2
#define left 3
#define right 4
#define action 5



#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "spi.h"
#include "dds.h"
//#include "dac.h"
//#include "lcd_lib.h"
//#include "ADC.h"

#define sbi(port_name, pin_number) (port_name |= 1<<pin_number)
#define cbi(port_name, pin_number) ((port_name) &= (uint8_t)~(1 << pin_number))
#define rbi(port_name, pin_number) ((port_name & (1 << pin_number)) >> pin_number)
#define FALSE 0
#define TRUE 1

#endif /* PROJECT_H_ */
