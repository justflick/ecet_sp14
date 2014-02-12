/*
 * serial.h
 *
 *  Created on: Jan 17, 2014
 *      Author: Justin Flick
 */

#ifndef SERIAL_H_
#define SERIAL_H_


uint8_t spiWriteByte(uint8_t data);
uint16_t spiWriteShort(uint16_t);
static volatile uint8_t txHead, txTail, txSerialInBuff[100];
static volatile uint8_t rxHead, rxTail, rxSerialInBuff[100];


void serialGetCmd(uint8_t *arg);
uint8_t serialInit(uint16_t baud);
void serialWriteString(const char *arg);
void SerialPutChar(uint8_t data);
void serialGetCmd(uint8_t *arg);

void spiInit();
void spiTransferIO(uint8_t * dataout, uint8_t * datain, uint8_t len);
void spiWriteString(uint8_t * dataout, uint8_t len);

//volatile uint8_t serialIndex = 0, serialBuff[70];

#endif /* SERIAL_H_ */
