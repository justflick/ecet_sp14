/*
 * dds.c
 *
 *  Created on: Jan 30, 2014
 *      Author: Justin Flick
 */

#include "main.h"
#include "dds.h"

/*appnote for maintaining sync'd clocks
 * http://www.analog.com/static/imported-files/application_notes/AN-605.pdf
 *
 * Datasheet for ad9833
 * http://www.analog.com/static/imported-files/data_sheets/AD9837.PDF
 *
 * Application paper
 * http://www.analog.com/library/analogdialogue/archives/38-08/dds.html
 */

void ad9833_init(void) {	//init both AD9833 units
	//initialize the SPI hardware
	DDS0_settings.bit=0;
	DDS1_settings.port=1;
	DDS_SPI_DDR |= ((1 << DDS0_settings.bit) | (1 << DDS1_settings.bit));
	SETBIT(DDS0_settings.port, DDS0_settings.bit);
	SETBIT(DDS1_settings.port, DDS1_settings.bit);
	_delay_us(10);	//wait before write as dictated by the ad9833 datasheet

	DDS0_settings.command_reg |= (1 << DDS_B28);
	CLEARBIT(DDS0_settings.port, DDS0_settings.bit);
	CLEARBIT(DDS1_settings.port, DDS1_settings.bit);

	spiWriteShort((1 << DDS_SLEEP12) | (1 << DDS_RESET));


	_delay_ms(100);
	DDS0_settings.command_reg |= (1 << DDS_SLEEP12);
	_delay_us(5);
	SETBIT(DDS0_settings.port, DDS0_settings.bit);
	SETBIT(DDS1_settings.port, DDS1_settings.bit);

	ad9833_set_frequency(0, 0);
//	ad9833_set_frequency(1, 0);
//	ad9833_set_phase(0, 0);
//	ad9833_set_phase(1, 0);
//	ad9833_set_freq_out(0);
//	ad9833_set_phase_out(0);

}

/**
 * Sets the ad9833 output waveform to the one given as a parameter.
 * \param mode possible values:
 *      - AD_OFF
 *      - AD_TRIANGLE
 *      - AD_SQUARE
 *      - AD_SINE
 */
void ad9833_set_mode(ad9833_settings_t* DDS_temp) {

//	DDS_temp.mode = mode;
	switch (DDS_temp->mode) {
		case DDS_OFF:
			DDS_temp->command_reg |= (1 << DDS_SLEEP12);
			DDS_temp->command_reg |= (1 << DDS_SLEEP1);
			break;
		case DDS_TRIANGLE:
			DDS_temp->command_reg &= (0 << DDS_OPBITEN);
			DDS_temp->command_reg |= (1 << DDS_MODE);
			DDS_temp->command_reg &= (0 << DDS_SLEEP12);
			DDS_temp->command_reg &= (0 << DDS_SLEEP1);
			break;
		case DDS_SQUARE:
			DDS_temp->command_reg |= (1 << DDS_OPBITEN);
			DDS_temp->command_reg &= (0 << DDS_MODE);
			DDS_temp->command_reg |= (1 << DDS_DIV2);
			DDS_temp->command_reg &= (0 << DDS_SLEEP12);
			DDS_temp->command_reg &= (0 << DDS_SLEEP1);
			break;
		case DDS_SINE:
			DDS_temp->command_reg &= (0 << DDS_OPBITEN);
			DDS_temp->command_reg &= (0 << DDS_MODE);
			DDS_temp->command_reg &= (0 << DDS_SLEEP12);
			DDS_temp->command_reg &= (0 << DDS_SLEEP1);
			break;
	}

	CLEARBIT(DDS_temp->port, DDS_temp->bit);
	_delay_us(5);
	spiWriteShort(DDS_temp->command_reg);
	_delay_us(5);
	SETBIT(DDS_temp->port, DDS_temp->bit);
}
/**
 * sets the desired ad9833 internal frequency register to a value that
 * produces the desired frequency.
 *
 * \param reg the desired frequency register to be manipulated, either 0 or 1
 * \param freq the desired frequency
 */
void ad9833_set_frequency(uint8_t reg, double freq) {
//        uint32_t freq_reg;
        uint16_t reg_reg;    //probably should be renamed...
//        freq_reg = AD_FREQ_CALC(freq);
        DDS0_settings.freq[reg] = freq;

        if (reg == 1)
                reg_reg = AD_FREQ1;
        else reg_reg = AD_FREQ0;

        CLEARBIT(DDS0_settings.port, DDS0_settings.bit);
        _delay_us(5);
//        ad9833_send((1 << DDS_B28) | DDS0_settings.command_reg);
//        ad9833_send(reg_reg | (0x3FFF & (uint16_t) (freq_reg >> 2)));
//        ad9833_send(reg_reg | (0x3FFF & (uint16_t) (freq_reg >> 16)));
        _delay_us(5);
        SETBIT(DDS0_settings.port, DDS0_settings.bit);}
/**
 * sets the desired ad9833 internal phase register to a value that
 * produces the desired phase.
 *
 * \param reg the desired phase register to be manipulated, either 0 or 1
 * \param phase the desired phase
 */
void ad9833_set_phase(uint8_t reg, double phase) {
        uint16_t registerTemp;    //probably should be renamed...
        if (reg == 1)
                registerTemp = AD_PHASE1;
        else registerTemp = AD_PHASE0;

        DDS0_settings.phase[reg] = phase;

//        AD_FSYNC_LO();
        _delay_us(5);
//        ad9833_send(registerTemp | AD_PHASE_CALC(DDS0_settings.phase[reg]));
        _delay_us(5);
//        AD_FSYNC_HI();
}


