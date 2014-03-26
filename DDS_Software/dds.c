/*
 * dds.c
 *
 *  Created on: Jan 30, 2014
 *      Author: Justin Flick
 */

#include "main.h"
#include "globals.h"
//#include "dds.h"

/**
 *
 * @param *ad5204 is a pointer to type ad5204 settings.
 * This struct contains the pin used for the SPi sync
 * signal, as well as the 8bit values to be loaded into
 * DACs 1-4.
 */
void ad5204SetVal(ad5204_settings_t *ad5204) {

	SpiInit(0, 0);  //spi init (must be done every write because of CPOL)
	CLEARBIT(PORTC, ad5204->pin);	//sync clear. 5ns is required.
	spiWriteByte(ad5204->port1);
	spiWriteByte(ad5204->port2);  //4 byte spi write
	spiWriteByte(ad5204->port3);
	spiWriteByte(ad5204->port4);
	SETBIT(PORTC, ad5204->pin);	//sync set

}

/*appnote for maintaining sync'd clocks
 * http://www.analog.com/static/imported-files/application_notes/AN-605.pdf
 *
 * Datasheet for ad9833
 * http://www.analog.com/static/imported-files/data_sheets/AD9837.PDF
 *
 * Application paper
 * http://www.analog.com/library/analogdialogue/archives/38-08/dds.html
 */

/**
 * Sets the ad9833 output waveform to the one given as a parameter.
 * \param mode possible values:
 *      - AD_OFF
 *      - AD_TRIANGLE
 *      - AD_SQUARE
 *      - AD_SINE
 */
void ad9833_set_mode(uint8_t mode) {
	ddsDevices.mod_freq = mode;
	serialWriteString("\nFrequency =");
	serialWriteNum(ddsDevices.freq, 1);
	serialWriteString("\n");
//	uint16_t temp = 0;
	switch (mode) {
	case AD9833_OFF:
		ddsDevices.command_reg |= (1 << AD9833_SLEEP12);
		ddsDevices.command_reg |= (1 << AD9833_SLEEP1);
		break;
	case AD9833_TRIANGLE:
		serialWriteString("\nTriangle!\n");
		ddsDevices.command_reg &= ~(1 << AD9833_OPBITEN);
		ddsDevices.command_reg |= (1 << AD9833_MODE);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP12);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP1);
		break;
	case AD9833_SQUARE:
		ddsDevices.command_reg |= (1 << AD9833_OPBITEN);
		ddsDevices.command_reg &= ~(1 << AD9833_MODE);
		ddsDevices.command_reg |= (1 << AD9833_DIV2);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP12);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP1);
		break;
	case AD9833_SINE:
		ddsDevices.command_reg &= ~(1 << AD9833_OPBITEN);
		ddsDevices.command_reg &= ~(1 << AD9833_MODE);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP12);
		ddsDevices.command_reg &= ~(1 << AD9833_SLEEP1);
		break;
	}

//	temp|=(1<<AD9833_B28);
	CLEARBIT(PORTC, ddsDevices.pin[0]);
	CLEARBIT(PORTC, ddsDevices.pin[1]);
//ddsDevices.command_reg|=(1<<6)|(1<<1);
	_delay_us(5);
	spiWriteShort(ddsDevices.command_reg);
	_delay_us(5);
	SETBIT(PORTC, ddsDevices.pin[0]);
	SETBIT(PORTC, ddsDevices.pin[1]);
}

/**
 * Adjust SPI settings to match the parameters needed to
 * drive the AD9833.
 *
 * @param clock_polarity and clock_phase control the SPI clocking
 * for appropriate control of devices on a shared bus.
 */
void SpiInit(uint8_t clock_polarity, uint8_t clock_phase) {
	DDR_SPI |= ((1 << DD_MOSI) | (1 << DD_SS) | (1 << DD_SCK));

	SPCR = ((1 << SPE) |     // SPI Enable
			(0 << SPIE) |     // SPI Interupt Enable
			(0 << DORD) |     // Data Order (0:MSB first / 1:LSB first)
			(1 << MSTR) |     // Master/Slave select
			(0 << SPR1) |     // SPI Clock Rate
			(1 << SPR0) |     // SPI Clock Rate
			(clock_polarity << CPOL) |    // Clock Polarity (0:SCK low / 1:SCK hi when idle)
			(clock_phase << CPHA));  // Clock Phase (0:leading / 1:trailing edge sampling)
	SPSR = (0 << SPI2X);     // Double Clock Rate
}

void ad9833Init(void) {  //init both AD9833 units

//set the appropriate DDR and SPI modes
	SpiInit(1, 0);  //set the SPI clock to idle high with reverse polarity.

	DDRC |= (1 << PINC4) | (1 << PINC5) | (1 << PINC3);

	ddsDevices.freq = 600;
	ddsDevices.mode = AD9833_SQUARE;
	ddsDevices.command_reg |= (1 << AD9833_B28);
	ddsDevices.phase[0] = ddsDevices.phase[1] = 0;
	ddsDevices.pin[0] = PINC4;
	ddsDevices.pin[1] = PINC5;

	PORTC |= (1 << 4) | (1 << 5);
//	SETBIT(PORTC, ddsDevices.pin[0]);
//	SETBIT(PORTC, ddsDevices.pin[1]);
//	SETBIT(PORTC, PINC3);
	_delay_us(10);
	PORTC &= ~((1 << 4) | (1 << 5));
//	CLEARBIT(PORTC, ddsDevices.pin[0]);
//	CLEARBIT(PORTC, ddsDevices.pin[1]);
//	CLEARBIT(PORTC, PINC3);

	_delay_us(10);  //wait before write as dictated by the ad9833 datasheet

	spiWriteShort((1 << AD9833_SLEEP12) | (1 << AD9833_RESET));
//	uint16_t test=0x6869;
//	spiWriteShort(test);
//	ddsDevices.command_reg |= (1 << AD9833_SLEEP12);

	_delay_us(10);
//	while (1){;;}
	PORTC |= (1 << 4) | (1 << 5);
//	SETBIT(PORTC, ddsDevices.pin[0]);
//	SETBIT(PORTC, ddsDevices.pin[1]);
//	SETBIT(PORTC, PINC3);
//	ddsDevices_t temp = *devices;
	ad9833_set_frequency(ddsDevices.freq);
	ad9833_set_mode(AD9833_TRIANGLE);
	ad9833_set_phase(0);

//	ad9833_set_frequency(devices);
//	ad9833_set_phase(0, 0);
//	ad9833_set_phase(1, 0);
//	ad9833_set_freq_out(0,0);
//	ad9833_set_phase_out(0);

}

void analogAdjust(ad5204 *data) {
	//setSpiAD5204

}
/**
 * sets the ad9833 internal frequency register to a value that
 * produces the desired frequency.
 * Max Freq is 2^32-1, or 4,294,967,295 Hz
 *
 * \param device is the desired DDS unit to be changed
 * \param freq is the desired frequency in steps of 1/100th HZ
 */
void ad9833_set_frequency(uint32_t freq) {
	serialWriteString("\nupdate freq");
	ddsDevices.freq = freq;
	uint32_t freqTemp = (uint32_t) (((double) AD9833_2POW28 / (double) AD9833_CLK * freq) * 4); //Calculate frequ word as per ad9833 datasheet
	CLEARBIT(PORTC, ddsDevices.pin[0]);
	CLEARBIT(PORTC, ddsDevices.pin[1]);
//	ddsDevices.command_reg |= AD_FREQ0;

	_delay_us(5);
	spiWriteShort((1 << AD9833_B28) | ddsDevices.command_reg);
	spiWriteShort(AD_FREQ0 | (0x3FFF & (uint16_t) (freqTemp >> 2)));
	spiWriteShort(AD_FREQ0 | (0x3FFF & (uint16_t) (freqTemp >> 16)));
	_delay_us(5);  //hold time for the word to xmit and be held in the ad9833 sipo register
	SETBIT(PORTC, ddsDevices.pin[0]);
	SETBIT(PORTC, ddsDevices.pin[1]);

}
/**
 * sets the ad9833 internal phase register to a value that
 * produces the desired phase.
 *
 * \param reg the desired phase register to be manipulated, either 0 or 1
 * \param phase the desired phase
 */
void ad9833_set_phase(uint16_t phase) {

//	ddsDevices.phase[0] = phase;

	PORTC &= ~((1 << 4) | (1 << 5));
	_delay_us(5);
	spiWriteShort(AD_PHASE0 | AD_PHASE_CALC(phase));
	_delay_us(5);
	PORTC |= (1 << 4) | (1 << 5);

//        AD_FSYNC_HI();
}

