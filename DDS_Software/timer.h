/*
 * timer.h
 *
 *  Created on: Jan 28, 2014
 *      Author: Konecranes
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "main.h"

uint16_t volatile systemTicks; /*ticker delay counter*/
volatile int reload;
extern volatile  uint16_t msLast;



void timerInit(uint16_t usecs);
void delayTicker_ms(uint16_t ms);


#endif /* TIMER_H_ */
