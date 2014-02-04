/*file of interrupt service routines*/
#include "main.h"
volatile unsigned int ticks; /*ticker delay counter*/
static unsigned int reload;

/*init tick delay time*/
void init_ticker(unsigned int usecs) {
	reload = usecs / 10; /*calc reload number*/
	TCCR0A = 0b00000011; /*prescale = 64*/
	TCNT0 = reload; /*load for 1st time out*/
	TIMSK0 = 0x1; /*T0 OV enabled*/
	SREG = (SREG | 0x80); /*enable interrupts*/
} /*end init ticker*/

ISR(TIMER0_OVF_vect) {
	TCNT0 = 256 - reload; /*set for next tick*/
	if (ticks > 0) {
		ticks = ticks - 1; /*decrement tick counter*/
	} /*end tick decrementer*/
	//return (0);
} /*end timer0 OVF isr*/
