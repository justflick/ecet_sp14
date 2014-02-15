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
#include <stdio.h>
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
#define DD_SS       DDB2
#define DD_SCK      DDB5
void spiInit() {
	DDR_SPI &= ~((1 << DD_MOSI) | (1 << DD_MISO) | (1 << DD_SS) | (1 << DD_SCK));
	DDR_SPI |= ((1 << DD_MOSI) | (1 << DD_SS) | (1 << DD_SCK));

	SPCR = ((1 << SPE) |     // SPI Enable
			(0 << SPIE) |     // SPI Interupt Enable
			(0 << DORD) |     // Data Order (0:MSB first / 1:LSB first)
			(1 << MSTR) |     // Master/Slave select
			(0 << SPR1) |     //j
			(0 << SPR0) |     // SPI Clock Rate
			(1 << CPOL) |     // Clock Polarity (0:SCK low / 1:SCK hi when idle)
			(0 << CPHA));     // Clock Phase (0:leading / 1:trailing edge sampling)
	SPSR = (0 << SPI2X);     // Double Clock Rate
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

uint8_t serialInit(uint16_t baud) {

	txHead = 0;
	txTail = 0;
	rxHead = 0;
	rxTail = 0;

	uint16_t baudCalc = ((F_CPU) + 8UL * baud) / (16UL * (baud) - 1UL);
	if (baudCalc > 0x8000) {
		UCSR0A = (0 << U2X0);  //set usart to 2x mode
		baudCalc &= 0x8000;
	} else {
		UCSR0A = (1 << U2X0);
	}
	UBRR0H = (uint8_t) baudCalc >> 8;  //set reigsters for correct usart speed.
	UBRR0L = (uint8_t) baudCalc;

	UCSR0A |= (1 << RXCIE0);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0)| (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	sei();

	if (baud != 0) {
		return 0;

	} else return 1;
}

void serialGetCmd(uint8_t *arg) {
	//parse serialInBuff for cmd and data strings
//	if serialInBuff[head]==
	while (txHead != txTail) {
		switch (txSerialBuff[txHead++]) {
			case cmd_ena:
				//do stuff
				txTail++;
				break;
			case cmd_dis:
				//do stuff
				txTail++;

				break;
			default:
				break;
		}
	}
}

ISR(USART_RX_vect) {
	rxHead = (rxHead + 1) % SERIAL_BUFFER_LEN;
	rxSerialBuff[rxHead] = UDR0;
  //prevent index from going OOB

}
void serialWriteString(const char *string) {
//	uint8_t serHead;
//	serHead=(txHead+1 )

	while (*string) {
		SerialPutChar(*string);
		string++;
	}

}

void serialWriteNum(uint8_t arg) {
	SerialPutChar('0' + (arg / 100) % 10);
	SerialPutChar('0' + (arg / 10) % 10);
	SerialPutChar('0' + (arg % 10));
	SerialPutChar('\n');
}

void SerialPutChar(uint8_t data) {
	while (!(UCSR0A & (_BV(UDRE0))))
		;  //Empty buffer
	UDR0 = data;
}

uint8_t serialGetChar(uint8_t *rxChar) {
	unsigned char tmpTime;
	tmpTime = ticks;


	while (rxHead == rxTail) {

		if (ticks > (tmpTime + 250)) {
			return RX_TIMEOUT;
		}
	}
//	serialWriteString("getchar ticks=");
//	serialWriteNum(ticks);

	*rxChar = UDR0;
	rxTail = (rxTail + 1) % SERIAL_BUFFER_LEN;
//	serialWriteString("\nrxhead=");
//	serialWriteNum(rxHead);
//	serialWriteString(",  rxtail=");
//	serialWriteNum(rxTail);
	return RX_SUCCESS;

	/* if (rxHead==rxTail)
	 * set millis
	 * loop while millis <timeoutMax
	 * return rx_timeout
	 * else
	 * 	rxchar=rxBuff[rxTail++];
	 * 	return rxCharSuccess
	 *
	 */
}

void serialWriteCmd(uint8_t *arg) {
//	register uint8_t controlCode;
//	while ((controlCode = *arg++) != 0) {
//
//	}
}

