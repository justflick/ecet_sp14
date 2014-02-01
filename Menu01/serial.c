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

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SS       DDB2	//comment
#define DD_SCK      DDB5

void spiInit() {
	DDR_SPI &= ~((1 << DD_MOSI) | (1 << DD_MISO) | (1 << DD_SS) | (1 << DD_SCK));
	DDR_SPI |= ((1 << DD_MOSI) | (1 << DD_SS) | (1 << DD_SCK));

	SPCR = ((1 << SPE) |     // SPI Enable
			(0 << SPIE) |    // SPI Interupt Enable
			(0 << DORD) |    // Data Order (0:MSB first / 1:LSB first)
			(1 << MSTR) |    // Master/Slave select
			(0 << SPR1) |    //j
			(0 << SPR0) |    // SPI Clock Rate
			(1 << CPOL) |    // Clock Polarity (0:SCK low / 1:SCK hi when idle)
			(0 << CPHA));    // Clock Phase (0:leading / 1:trailing edge sampling)
	SPSR = (0 << SPI2X);              // Double Clock Rate
}

void spiTransferIO(uint8_t * dataout, uint8_t * datain, uint8_t len) {
	//takes the pointer to an array and writes incoming bytes incrementally
	//no bounds checking, so long reads can meander through the heap if no care is taken.
	uint8_t i;
	for (i = 0; i < len; i++) {
		SPDR = dataout[i];
		while ((SPSR & (1 << SPIF)) == 0)
			;
		datain[i] = SPDR;
	}
}

void spiWriteString(uint8_t * dataout, uint8_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		SPDR = dataout[i];
		while ((SPSR & (1 << SPIF)) == 0)
			;
	}
}

uint8_t spiWriteByte(uint8_t byteout) {
	/**writes 8 bits and returns the value shifted into SPDR*/
//      cbi(PORTB,2);
	SPDR = byteout;
	while ((SPSR & (1 << SPIF)) == 0)
		;
//      sbi(PORTB,2);
	return SPDR;
}

uint16_t spiWriteShort(uint16_t data) {

	SPDR = (data >> 8);
	while ((SPSR & (1 << SPIF)) == 0) {
	}
	SPDR = data & 0x00ff;
	while ((SPSR & (1 << SPIF)) == 0) {
	}

	return data;

}

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
