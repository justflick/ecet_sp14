/*
 * dds.c
 *
 *  Created on: Jan 30, 2014
 *      Author: Justin Flick
 */

#include "main.h"
#include "globals.h"
//#include "dds.h"

/*appnote for maintaining sync'd clocks
 * http://www.analog.com/static/imported-files/application_notes/AN-605.pdf
 *
 * Datasheet for ad9833
 * http://www.analog.com/static/imported-files/data_sheets/AD9837.PDF
 *
 * Application paper
 * http://www.analog.com/library/analogdialogue/archives/38-08/dds.html
 */

void ad9833Init(ad9833_settings_t *devices) {  //init both AD9833 units
	//initialize the SPI hardware
	devices->port = PORTC;
	devices->pin[0] = PINC4;
	devices->pin[1] = PINC5;
	DDS_SPI_DDR |= ((1 << devices->pin[0]) | (1 << devices->pin[1]));
	SETBIT(devices->port, devices->pin[0]);
	SETBIT(devices->port, devices->pin[1]);
	_delay_us(10);	//wait before write as dictated by the ad9833 datasheet

	devices->reg[0] |= (1 << DDS_B28);
	devices->reg[1] |= (1 << DDS_B28);

	CLEARBIT(devices->port, devices->pin[0]);
	CLEARBIT(devices->port, devices->pin[1]);

	spiWriteShort((1 << DDS_SLEEP12) | (1 << DDS_RESET));

	_delay_ms(10);	//change to 100 when delay_ms fixed
	devices->reg[1] |= (1 << DDS_SLEEP12);
	devices->reg[0] |= (1 << DDS_SLEEP12);
	_delay_us(5);
	SETBIT(devices->port, devices->pin[0]);
	SETBIT(devices->port, devices->pin[1]);

	devices->freq = 400;	//set initial frequency to 400Hz
	ad9833_set_frequency(devices);
//	ad9833_set_frequency(1, 0);
//	ad9833_set_phase(0, 0);
//	ad9833_set_phase(1, 0);
//	ad9833_set_freq_out(0);
//	ad9833_set_phase_out(0);

}

void analogAdjust(ad5204 *data) {
	//setSpiAD5204

}
/**
 * Sets the ad9833 output waveform to the one given as a parameter.
 * \param mode possible values:
 *      - AD_OFF
 *      - AD_TRIANGLE
 *      - AD_SQUARE
 *      - AD_SINE
 */
void ad9833_set_mode(ad9833_settings_t* devices) {

//	DDS_temp.mode = mode;
	switch (devices->mode[0]) {
		case DDS_OFF:
			devices->command_reg[0] |= (1 << DDS_SLEEP12);
			devices->command_reg[0] |= (1 << DDS_SLEEP1);
			break;
		case DDS_TRIANGLE:
			devices->command_reg[0] &= (0 << DDS_OPBITEN);
			devices->command_reg[0] |= (1 << DDS_MODE);
			devices->command_reg[0] &= (0 << DDS_SLEEP12);
			devices->command_reg[0] &= (0 << DDS_SLEEP1);
			break;
		case DDS_SQUARE:
			devices->command_reg[0] |= (1 << DDS_OPBITEN);
			devices->command_reg[0] &= (0 << DDS_MODE);
			devices->command_reg[0] |= (1 << DDS_DIV2);
			devices->command_reg[0] &= (0 << DDS_SLEEP12);
			devices->command_reg[0] &= (0 << DDS_SLEEP1);
			break;
		case DDS_SINE:
			devices->command_reg[0] &= (0 << DDS_OPBITEN);
			devices->command_reg[0] &= (0 << DDS_MODE);
			devices->command_reg[0] &= (0 << DDS_SLEEP12);
			devices->command_reg[0] &= (0 << DDS_SLEEP1);
			break;
	}

	CLEARBIT(devices->port, devices->pin[0]);
	_delay_us(5);
	spiWriteShort(devices->command_reg[0]);
	_delay_us(5);
	SETBIT(devices->port, devices->pin[0]);
}
/**
 * sets the ad9833 internal frequency register to a value that
 * produces the desired frequency.
 * Max Freq is 2^32-1, or 4,294,967,295 Hz
 *
 * \param device is the desired DDS unit to be changed
 * \param freq is the desired frequency in steps of 1/100th HZ
 */
void ad9833_set_frequency(ad9833_settings_t *devices) {

	AD9833SpiInit();	//reset required SPI mode since the bus is shared with other devices.
	uint32_t freqTemp = (uint32_t) (((double) DDS_2POW28 / (double) DDS_CLK * devices->freq) * 4);  //Calculate frequ word as per ad9833 datasheet
	CLEARBIT(devices->port, devices->pin[0]);
	CLEARBIT(devices->port, devices->pin[1]);

	_delay_us(5);
	spiWriteShort((1 << DDS_B28) | devices->command_reg[0]);
	spiWriteShort(devices->reg[0] | (0x3FFF & (uint16_t) (freqTemp >> 2)));
	spiWriteShort(devices->reg[0] | (0x3FFF & (uint16_t) (freqTemp >> 16)));
	_delay_us(5);  //hold time for the word to xmit and be held in the ad9833 sipo register
	SETBIT(devices->port, devices->pin[0]);
	SETBIT(devices->port, devices->pin[1]);
}
/**
 * sets the ad9833 internal phase register to a value that
 * produces the desired phase.
 *
 * \param reg the desired phase register to be manipulated, either 0 or 1
 * \param phase the desired phase
 */
void ad9833_set_phase(ad9833_settings_t *devices, uint32_t phase) {

	devices->phase[0] = phase;

//        AD_FSYNC_LO();
	_delay_us(5);
//        ad9833_send(registerTemp | AD_PHASE_CALC(DDS0_settings.phase[reg]));
	_delay_us(5);
//        AD_FSYNC_HI();
}

