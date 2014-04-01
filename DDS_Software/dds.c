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
void ad5204SetVal(uint8_t value, uint8_t address) {
	SpiInit(0, 0);	//adjust the SPI clock polarity for ad5204 use
	DDRC |= (1 << 3) | (1 << 2);

	uint8_t pinTemp = 3;
	if (address > 3) {
		address -= 4;
		pinTemp = 2;
	}
	CLEARBIT(PORTC, pinTemp);	//sync clear. 5ns is required.
	spiWriteByte(address);
	spiWriteByte(value);
	SETBIT(PORTC, pinTemp);	//sync set

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
void ad9833_set_mode(uint8_t mode, uint8_t dds0, uint8_t dds1) {
	SpiInit(1, 0);
	_delay_us(20);

	ddsDevices.mod_freq = mode;
	switch (mode) {
	case AD9833_OFF:
		ddsDevices.command_reg |= (1 << AD9833_SLEEP12);
		ddsDevices.command_reg |= (1 << AD9833_SLEEP1);
		break;
	case AD9833_TRIANGLE:
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
//	CLEARBIT(PORTC, ddsDevices.pin[ddsSelect]);
//ddsDevices.command_reg|=(1<<6)|(1<<1);
	PORTC &= ~((dds0 << 4) | (dds1 << 5));
	_delay_us(5);
	spiWriteShort(ddsDevices.command_reg);
	_delay_us(5);
	PORTC |= (dds0 << 4) | (dds1 << 5);

//	SETBIT(PORTC, ddsDevices.pin[ddsSelect]);
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
	_delay_ms(1);
}

void ad9833Init(void) {  //init both AD9833 units

	SpiInit(1, 0);  //set the SPI clock to idle high with reverse polarity.
	_delay_us(20);

	DDRC |= (1 << PINC4) | (1 << PINC5) | (1 << PINC3);
	ddsDevices.freq = 1000;
	ddsDevices.mode = AD9833_SQUARE;
	ddsDevices.command_reg |= (1 << AD9833_B28);
	ddsDevices.phase[0] = ddsDevices.phase[1] = 0;
	ddsDevices.pin[0] = PINC4;
	ddsDevices.pin[1] = PINC5;

	PORTC |= (1 << 4) | (1 << 5);
	_delay_ms(1);
	PORTC &= ~((1 << 4) | (1 << 5));
	_delay_us(10);  //wait before write as dictated by the ad9833 datasheet
	spiWriteShort((1 << AD9833_SLEEP12) | (1 << AD9833_RESET));
	_delay_us(10);
	PORTC |= (1 << 4) | (1 << 5);

	PORTC &= ~((1 << 4) | (1 << 5));
	_delay_us(10);  //wait before write as dictated by the ad9833 datasheet
	spiWriteShort(ddsDevices.command_reg);
	_delay_us(10);
	PORTC |= (1 << 4) | (1 << 5);

	ad5204SetVal(255, 0);

	_delay_ms(10);

	ad9833_set_mode(AD9833_SQUARE, 1, 1);
//	ad9833_set_mode(ddsDevices.mode, 0,1);
	_delay_ms(10);

	ad9833_set_frequency(ddsDevices.freq);
	_delay_ms(10);
	ad9833_set_phase(0, 1, 1);	//phase degrees

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
	SpiInit(1, 0);	//change SPI mode to AD983 compatible mode.
	spiWriteByte(0);	//Flush DDS SPI inputs


	ddsDevices.freq = freq;
	uint32_t freqTemp = (uint32_t) (((double) AD9833_2POW28 / (double) AD9833_CLK * (freq + 1)) * 4); //Calculate frequ word as per ad9833 datasheet
	PORTC &= ~((1 << 4) | (1 << 5));

	_delay_us(10);
//	spiWriteShort((1 << AD9833_B28) | ddsDevices.command_reg);
	spiWriteShort(AD_FREQ0 | (0x3FFF & (uint16_t) (freqTemp >> 2)));
	spiWriteShort(AD_FREQ0 | (0x3FFF & (uint16_t) (freqTemp >> 16)));
	_delay_us(10);  //hold time for the word to xmit and be held in the ad9833 sipo register
	PORTC |= (1 << 4) | (1 << 5);

}
/**
 * sets the ad9833 internal phase register to a value that
 * produces the desired phase.
 *
 * \param reg the desired phase register to be manipulated, either 0 or 1
 * \param phase the desired phase
 */
void ad9833_set_phase(int16_t phase, uint8_t ad0, uint8_t ad1) {
	SpiInit(1, 0);  //set the SPI clock to idle high with reverse polarity.
	_delay_us(20);

	phase = (uint32_t) (512) * phase / 45; //must cast to prevent overlfow at signed 8bit int

	PORTC &= ~((ad0 << 4) | (ad1 << 5));
	_delay_us(5);
	spiWriteShort(AD_PHASE0 | phase);
	_delay_us(5);
	PORTC |= (ad0 << 4) | (ad1 << 5);
}

