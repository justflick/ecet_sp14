/*file of interrupt service routines*/
#include "main.h"
#include "timer.h"


/*init tick delay time*/
uint8_t timerInit(uint16_t usecs) {
	CLEARBIT(DDRD,4);
	SETBIT(PORTD,4);
	reload = usecs / 10; /*calc reload number*/
	TCCR0B = 0b00000011; /*prescale = 64*/
	TCNT0 = reload; /*load for 1st time out*/
	TIMSK0 |=(1<<TIMSK0); /*T0 OV enabled*/
	SREG |= (SREG | 0x80); /*enable interrupts*/
//sei();
	return 1;
} /*end init ticker*/



ISR(TIMER0_OVF_vect) {
//serialWriteString("\nticker Fired");
	TCNT1 = 256 - reload; /*set for next tick*/
	systemTicks++;
//	if (ticks > 0) {
//		ticks = ticks - 1; /*decrement tick counter*/
//	} /*end tick decrementer*/

}
