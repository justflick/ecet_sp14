/*
 * serial.c
 *
 *  Created on: Jan 17, 2014
 *      Author: Justin Flick
 *
 *  Description: Serial tranceiver and string parsing
 *  Default Parameters: 9600baud, 8 bit, HW flow ON
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#define received 0x00
#define notRx 0xf0
#define error 0xf1
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define bufferLen 40

void serialInit(uint8_t *arg) {
	UCSR0A = (0 << U2X0);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);  //sadflsls sdfls
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	sei();
}

void serialGetCmd(uint8_t *arg) {
	uint8_t status = 0;

}

ISR(USART_RX_vect, ISR_BLOCK) {
	volatile uint8_t head = 0, tail = 0;
	volatile uint8_t inputBuff[bufferLen];
	volatile uint8_t outputBuff[40];
	inputBuff[head] = UDR0;
	if (++head > bufferLen) head = 0;

}

void writeSerial(uint8_t *arg) {
	register uint8_t controlCode;
	while ((controlCode = *arg++) != 0) {

	}
}
