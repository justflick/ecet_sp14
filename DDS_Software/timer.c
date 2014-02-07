/*file of interrupt service routines*/
#include "main.h"
#include "timer.h"


/*init tick delay time*/
uint8_t timerInit(uint16_t usecs) {
	reload = usecs / 10; /*calc reload number*/
	TCCR0A = 0b00000011; /*prescale = 64*/
	TCNT0 = reload; /*load for 1st time out*/
	TIMSK0 = 0x1; /*T0 OV enabled*/
	SREG = (SREG | 0x80); /*enable interrupts*/

	return 1;
} /*end init ticker*/



ISR(TIMER0_OVF_vect) {

	TCNT0 = 256 - reload; /*set for next tick*/
	if (ticks > 0) {
		ticks = ticks - 1; /*decrement tick counter*/
	} /*end tick decrementer*/

}
