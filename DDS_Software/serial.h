/*
 * serial.h
 *
 *  Created on: Jan 17, 2014
 *      Author: Justin Flick
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#define RX_SUCCESS 0x00
#define RX_TIMEOUT 0xf0
#define RX_FAIL 0xff

#define TX_SUCCESS 0x00
#define TX_BUFF_FULL 0xf0

#define SERIAL_BUFFER_LEN 30
uint8_t spiWriteByte(uint8_t data);
uint16_t spiWriteShort(uint16_t);
volatile uint8_t txHead, txTail, txSerialBuff[100];
volatile uint8_t rxHead, rxTail, rxSerialBuff[100];


void serialGetCmd(uint8_t *arg);
void serialWriteNum(uint8_t arg);
uint8_t serialInit(uint16_t baud);
void serialWriteString(const char *arg);
void SerialPutChar(uint8_t data);
uint8_t serialGetChar(uint8_t *rxChar);
void serialGetCmd(uint8_t *arg);

void spiInit();
void spiTransferIO(uint8_t * dataout, uint8_t * datain, uint8_t len);
void spiWriteString(uint8_t * dataout, uint8_t len);

//volatile uint8_t serialIndex = 0, serialBuff[70];

#endif /* SERIAL_H_ */