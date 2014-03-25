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
	uint8_t reg[2];  //Valid range is 0-1
	uint8_t mode;
	uint16_t command_reg;
	uint8_t port;
	uint8_t pin[2];

} ddsDevices_t;

/**
 * @struct Contains the DDS parameters used for driving peripherals
 *
 */
typedef struct {
	uint8_t port1, port2, port3, port4,pin;

} ad5204_settings_t;

volatile ad5204_settings_t dac0, dac1;

/**
 * @struct Nested Struct for interactive parameters such as
 * front panel values, serial commands and values, user settings etc.
 *  */

typedef struct {
	uint8_t offset1, offset2,  //DPOT 0 and 1 cascaded for offset control
			gain1, gain2;  //DPOT 0 and 1 cascased for 512bit gain control
} ad5204;
typedef struct {
	int32_t min, max, currentValue;
	uint8_t digits, decimal, decade, changed;
} parameter_defs;

typedef struct {
	parameter_defs Hz, period, vMax, offset, vMin, dutyCycle, VPP, vRMS, phase, PWM;
	uint8_t waveShape;  //0-2 used to represent sine, square, ramp.
} userParam_t;

userParam_t userParameters;

volatile ddsDevices_t DDS0_settings, DDS1_settings;
volatile uint16_t systemTicks; /*ticker delay counter*/
volatile uint8_t pbVal;
volatile ddsDevices_t ddsDevices;

void parameterInit(userParam_t *structAddress);
#endif /* GLOBALS_H_ */
