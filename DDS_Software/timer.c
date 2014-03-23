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
	TCCR0A=0;
	TCCR0B = 0;
	TCNT0 = 0;
	OCR0A=124;// = (F_CPU) / (usecs*64) - 1
	TCCR0A |= (1<<WGM01); //waveform genMode
	TCCR0B |=(1<<CS01)|(1<<CS00);	//enable 64clk prescale
	TIMSK0 |= (1 << OCIE0A); /*T0 Interrupt enabled*/
	sei();
	return 0;
} /*end init ticker*/


/**
 * Ticker routine increment based on comapre match
 * @param TIMER0_COMPA_vect
 */
ISR(TIMER0_COMPA_vect) {
//serialWriteString("\nticker Fired");
	systemTicks++;
//	if (systemTicks==0xffff) systemTicks=0;
}
