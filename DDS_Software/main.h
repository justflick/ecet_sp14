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

#include "joystick.h"
#include "lcd.h"
#include "serial.h"
#include "tinymenu.h"

#define cmd_ofs "o"
#define cmd_vpp "v"
#define cmd_frq "f"
#define cmd_per "p"
#define cmd_vpk "k"
#define cmd_vmn "m"
#define cmd_sqa "q"
#define cmd_tri "t"
#define cmd_sin "s"
#define cmd_dut "c"

/*! packet structure:
 *
 * [letter]:3 digit magnitude:1 digit range
 *
 * Exmaple:
 *  s:261:0
 *  yields a square wave with 458Hz
 */

#define JOYSTICK_NOPRESS 0
#define JOYSTICK_UP 1
#define JOYSTICK_DOWN 2
#define JOYSTICK_LEFT 3
#define JOYSTICK_RIGHT 4
#define JOYSTICK_ENTER 5

#define DDS_SPI_DDR PORTB

#define SETBIT(PORT,BIT) (PORT |= (1<<BIT))
#define CLEARBIT(PORT,BIT) (PORT &= ~(1<<BIT))
#define FLIPBIT(PORT,BIT) (PORT ^= (1<<BIT))



//begin function prototypes for main.c
void my_select(void *arg, char *name);
void adjust_value(void *arg,char  *value);





#endif /* MAIN_H_ */
