/*
 * globals.h
 *
 *  Created on: Feb 19, 2014
 *      Author: Konecranes
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

/** Struct that holds all the configuration it's initialized as a global variable
 * in the ad9833.c file */
typedef struct {
	long freq;  //<Holds the frequencies of
	long phase[2];
	long mod_freq;
	uint8_t reg[2];	//Valid range is 0-1
	uint8_t mode[2];
	uint16_t command_reg[2];
	uint8_t port;
	uint8_t pin[2];
} ad9833_settings_t;

/**
 * @struct Contains the DDS parameters used for driving peripherals
 *
 */

/**
 * @struct Nested Struct for interactive parameters such as
 * front panel values, serial commands and values, user settings etc.
 *  */

typedef struct {
	uint8_t offset1, offset2,  //DPOT 0 and 1 cascaded for offset control
			gain1, gain2;  //DPOT 0 and 1 cascased for 512bit gain control
} ad5204;
typedef struct {
	uint32_t min, max, currentValue;
	uint8_t digits, decimal, decade, changed;
} parameter_defs;

typedef struct {
	parameter_defs Hz, period, offset, gain, vMax, vMin, VPP, vRMS, phase, dutyCycle, PWM;
	uint8_t waveShape;  //0-2 used to represent sine, square, ramp.

} userParam_t;

userParam_t userParameters;

//extern volatile ad9833_settings_t DDS0_settings, DDS1_settings;
extern volatile uint16_t systemTicks; /*ticker delay counter*/

void parameterInit(userParam_t *structAddress);
#endif /* GLOBALS_H_ */
