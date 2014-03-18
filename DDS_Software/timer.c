/*file of interrupt service routines*/
#include "main.h"
//#include "timer.h"



/**
 * interrupt driven ticker
 * @param ms
 */
void delayTicker_ms(uint16_t ms) {
	uint16_t tmpTimer = systemTicks;
	while ((tmpTimer + ms) > systemTicks) {

	}
}

/*init tick delay time*/
uint8_t timerInit(uint16_t usecs) {
	reload = usecs/ 10; /*calc reload number*/
	TCCR0B = 0b00000011; /*prescale = 64*/
	TCNT0 = reload; /*load for 1st time out*/
	TIMSK0 |=(1<<TIMSK0); /*T0 OV enabled*/
	SREG |= (SREG | 0x80); /*enable interrupts*/
	return 0;
} /*end init ticker*/



ISR(TIMER0_OVF_vect) {
//serialWriteString("\nticker Fired");
	TCNT0 = 231 - reload; /*set for next tick*/
	systemTicks++;
}
