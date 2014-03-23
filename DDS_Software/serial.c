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

#include "main.h"
#define received 0x00
#define notRx 0xf0
#define error 0xf1
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define bufferLen 40

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

	return SPDR;//SPI Peripheral data register will contain any bits received on MISO pin.

}

uint8_t serialInit(uint32_t baud) {
	uint16_t baudCalc = ((F_CPU) / (8UL * baud)) - 1UL;

	UBRR0H = (baudCalc >> 8);
	UBRR0L = baudCalc;
	UCSR0A |= (0 << RXC0) | (1 << U2X0);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);	//setup for 8 bit data

//	sei();
//	UDR0 = 'Q';
	txHead = 0;
	txTail = 0;
	rxHead = 0;
	rxTail = 0;
	return 1;
}

/*! Function for parsing incoming packets
 * Arguments
 *
 * packet structure:
 *
 * 9 bytes
 * \esc, \stx, control, longval, checksum, \etx
 *
 */

void serialGetCmd(uint8_t *arg) {
	//parse serialInBuff for cmd and data strings
	uint8_t tempCmd[30], i = 0;

	while (tempCmd[0] != 0x1B) { //seek for escapement byte
		serialGetChar(&tempCmd[0], 1, 200);
		if (++i == 25)
			break;
	}
	serialGetChar(&tempCmd[0], 9, 200);	//receive: startbit, control, longVal, checksum,endbit =bytes
	if (tempCmd[1] != 0x02)
		return;
	switch (tempCmd[2]) {
	case 'f':

		break;
	case 'p':
		break;
	case 'm':
	default:
		break;
	}

}

ISR(USART_RX_vect) {
	//called when RX register receives a byte.
	rxHead = (rxHead + 1) % SERIAL_BUFFER_LEN;
	rxSerialBuff[rxHead] = UDR0;
	//prevent index from going OOB

}

ISR(USART_UDRE_vect) {
	//called when tx register is empty
//	SerialPutChar(txSerialBuff[txHead]);
	if (txHead != txTail) {
		txTail = (txTail + 1) % SERIAL_BUFFER_LEN;
		UDR0 = txSerialBuff[txTail];	//xmit current char
	} else {
		while (!(UCSR0A & _BV(TXC0))) {
		}
		//wait before last byte is out before clearing IE flag! 4 evenings wasted on this! AAAGGGHHHHH!!!!
		UCSR0B &= ~(1 << TXCIE0);
		UCSR0B &= ~(1 << UDRIE0);	//turn interrupt off
	}
}
uint8_t serialWriteString(char *string) {
	while (*string) {
		serialPutChar(*string++);
	}
	return TX_SUCCESS;
}
/**
 * Takes a number and prints it as a 5 digit string.
 * @param arg
 */
void serialWriteNum(uint32_t arg, uint8_t len) {
	serialPutChar('0' + (arg / 10000) % 10);
	serialPutChar('0' + (arg / 1000) % 10);
	serialPutChar(',');
	serialPutChar('0' + (arg / 100) % 10);
	serialPutChar('0' + (arg / 10) % 10);
	serialPutChar('0' + (arg % 10));
}
/*!===========================================*
 *	FUNCTION: Serial Put Char
 *	Purpose: Places characters into
 *	the usart transmit buffer
 *
 *	Parameter: fixed length unsigned char
 *
 *	Return: Void
 *=============================================
 */
uint8_t serialPutChar(uint8_t data) {
	uint16_t tmpHead;
	tmpHead = (txHead + 1) % SERIAL_BUFFER_LEN;
	while (tmpHead == txTail) {
	}
	txSerialBuff[tmpHead] = data;
	txHead = tmpHead;
	UCSR0B |= (1 << UDRIE0);
	return TX_SUCCESS;

}

void serialPutStringImmediate(const char *data) {

	while (*data) {
		while (!(UCSR0A & (_BV(UDRE0))))
			;  //Empty buffer
		UDR0 = *data++;
	}
}
/**
 * uint8_t serialGetChar(uint8_t *rxChar, uint8_t len, uint8_t timeout)
 *
 * @param rxChar: Address where to place RX'd chars.
 * @param len: Number of characters to be received.
 * @param timeout: number of milliseconds (ticks) to wait before returning RX_TIMEOUT
 * @return returns 0 on RX success
 *
 * @brief SerialGetChar takes three arguments and retrieves len number of
 * characters from the serial buffer within timeout milliseconds.
 * The buffer is fed values from the usart_rx interrupt routine, which by default, will
 * store 32 bytes in the buffer before dropping incoming data.
 */
uint8_t serialGetChar(uint8_t *rxChar, uint8_t len, uint8_t timeout) {
	unsigned char tmpTime = systemTicks;

	while (rxHead == rxTail) {

		if ((systemTicks > (tmpTime + 100)) && (tmpTime != 0)) {
			return RX_TIMEOUT;
		}
	}
	if (len > SERIAL_BUFFER_LEN)
		len = SERIAL_BUFFER_LEN;	//bounds checking on length
	if (len == 0)
		len = 1;

	for (int i = 0; i < len; ++i) {	//retrieve len number of bytes from serial queue

		rxTail = (rxTail + 1) % SERIAL_BUFFER_LEN;
		*rxChar++ = rxSerialBuff[rxTail];
	}
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

