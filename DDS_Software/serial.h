/*
 * serial.h
 *
 *  Created on: Jan 17, 2014
 *      Author: Justin Flick
 */

#ifndef SERIAL_H_
#define SERIAL_H_

void serialGetCmd(uint8_t *arg);
void serialInit(uint8_t *arg);
void writeSerial(uint8_t *arg);
//void serialGetCmd(uint8_t *arg);

void spiInit();
void spiTransferIO(uint8_t * dataout, uint8_t * datain, uint8_t len);
void spiWriteString(uint8_t * dataout, uint8_t len);
uint8_t spiWriteByte(uint8_t data);
uint16_t spiWriteShort(uint16_t);
uint8_t head, tail, serialInBuff[100];

//volatile uint8_t serialIndex = 0, serialBuff[70];

#endif /* SERIAL_H_ */
