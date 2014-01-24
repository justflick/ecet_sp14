#include <avr/io.h>
#include <avr/interrupt.h>
#include "project.h"

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3                //11
#define DD_SS       DDB2                //10
#define DD_SCK      DDB5                //13
void spiInit()
// Initialize pins for spi communication
{
        DDR_SPI &= ~((1 << DD_MOSI) | (1 << DD_MISO) | (1 << DD_SS) | (1 << DD_SCK));
        DDR_SPI |= ((1 << DD_MOSI) | (1 << DD_SS) | (1 << DD_SCK));

        SPCR = ((1 << SPE) |     // SPI Enable
                        (0 << SPIE) |    // SPI Interupt Enable
                        (0 << DORD) |    // Data Order (0:MSB first / 1:LSB first)
                        (1 << MSTR) |    // Master/Slave select
                        (0 << SPR1) |    //j
                        (1 << SPR0) |    // SPI Clock Rate
                        (1 << CPOL) |    // Clock Polarity (0:SCK low / 1:SCK hi when idle)
                        (0 << CPHA));    // Clock Phase (0:leading / 1:trailing edge sampling)
        //ad9833 confirmed@10010010
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

        SPDR = byteout;
        while ((SPSR & (1 << SPIF)) == 0)
                ;
        return SPDR ;
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
