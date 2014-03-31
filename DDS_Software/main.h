/*
 * main.h
 *
 *  Created on: Jan 22, 2014
 *      Author: Justin Flick
 *
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "globals.h"
#include "dds.h"
#include "inputs.h"
#include "lcd.h"
#include "timer.h"
#include "serial.h"
#include "tinymenu.h"
#include "tinymenu_hw.h"

#define cmd_ena 'e'	//enable#define cmd_dis 'd'
#define cmd_ofs 'o'
#define cmd_vpp 'v'
#define cmd_frq 'f'
#define cmd_per 'p'
#define cmd_vpk 'k'
#define cmd_vmn 'm'
#define cmd_sqa 'q'
#define cmd_tri 't'
#define cmd_sin 's'
#define cmd_dut 'c'

//the keypress definitions are mapped so that the numeric keypad can also be
//used to control the menu via serial console.
#define JOYSTICK_NOPRESS '0'
#define JOYSTICK_UP '8'
#define JOYSTICK_DOWN '2'
#define JOYSTICK_LEFT '4'
#define JOYSTICK_RIGHT '6'
#define JOYSTICK_ENTER '5'

#define Parameter_Hz 1
#define Parameter_Offset 2
#define Parameter_Period 3
#define Parameter_PWM 4
#define Parameter_VPP 5
#define Parameter_Vmax 6
#define Parameter_Vmin 7
#define Parameter_Vrms 8
#define Parameter_Phase 9
#define Parameter_DutyCycle 10

#define DDS_SPI_DDR PORTC
#define DDS0_SPI_PIN

#define SETBIT(PORT,BIT) (PORT |= (1<<BIT))
#define CLEARBIT(PORT,BIT) (PORT &= ~(1<<BIT))
#define FLIPBIT(PORT,BIT) (PORT ^= (1<<BIT))
#define CHECKBIT(PORT,BIT) (1&(1<<BIT))

//begin function prototypes for main.c
void waveType(void *arg, char *name);
void adjust_value(void *arg, char *value);

#endif /* MAIN_H_ */
