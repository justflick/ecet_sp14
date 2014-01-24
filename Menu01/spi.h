
#ifndef _SPI_H_
#define _SPI_H_
#include <avr/io.h>
#include "project.h"

void spiInit();
void spiTransferIO(uint8_t * dataout, uint8_t * datain, uint8_t len);
void spiWriteString(uint8_t * dataout, uint8_t len);
 uint8_t spiWriteByte(uint8_t data);
uint16_t spiWriteShort(uint16_t);

#endif /* _SPI_H_ */
