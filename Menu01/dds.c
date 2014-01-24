#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dds.h"
#include "spi.h"
#define ddsPort PORTC

ad9833_settings_t ad_settings;    //create instance ad_settigns of type structure ad9833_settigns_t

/**
 * a wrapper function for sending 16-bit SPI packets.
 * \param packet 16-bit value to be sent over SPI.
 */
static inline void ad9833_send(uint16_t packet) {
        spiWriteByte((uint8_t) (packet >> 8));
        spiWriteByte((uint8_t) packet);

}

/**
 * Initializes the AD9833 and the relevant variables.
 * Also initializes the Timer1 peripheral that is used for modulation timing.
 */
void ad9833_init(uint8_t portno) {
        //init FSYNC pin (aka Chip select)
        ad_settings.command_reg |= (1 << AD_B28);
        DDRC = 0xFF;
        sbi(ddsPort,portno);


        _delay_us(10);          //requried by ad9833 spec

        //start as half-asleep
        cbi(ddsPort,0);
        _delay_us(5);
        ad9833_send((1 << AD_SLEEP12) | (1 << AD_RESET));
        ad_settings.command_reg |= (1 << AD_SLEEP12);
        _delay_us(5);
        sbi(ddsPort,portno);

        ad9833_set_frequency(0,0,0);
        ad9833_set_frequency(0,1,0);
        ad9833_set_phase(0,0,0);
        ad9833_set_phase(0,1,0);
        ad9833_set_freq_out(0,0);
        ad9833_set_phase_out(0,0);

}

/**
 * Sets the ad9833 output waveform to the one given as a parameter.
 * \param mode possible values:
 *      - AD_OFF
 *      - AD_TRIANGLE
 *      - AD_SQUARE
 *      - AD_SINE
 */
void ad9833_set_mode(uint8_t portno, uint8_t mode) {
        ad_settings.mode = mode;
        switch (mode) {
                case AD_OFF:
                        ad_settings.command_reg |= (1 << AD_SLEEP12);
                        ad_settings.command_reg |= (1 << AD_SLEEP1);
                        break;
                case AD_TRIANGLE:
                        ad_settings.command_reg &= ~(1 << AD_OPBITEN);
                        ad_settings.command_reg |= (1 << AD_MODE);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP12);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP1);
                        break;
                case AD_SQUARE:
                        ad_settings.command_reg |= (1 << AD_OPBITEN);
                        ad_settings.command_reg &= ~(1 << AD_MODE);
                        ad_settings.command_reg |= (1 << AD_DIV2);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP12);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP1);
                        break;
                case AD_SINE:
                        ad_settings.command_reg &= ~(1 << AD_OPBITEN);
                        ad_settings.command_reg &= ~(1 << AD_MODE);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP12);
                        ad_settings.command_reg &= ~(1 << AD_SLEEP1);
                        break;
        }

        cbi(ddsPort,portno);
        _delay_us(50);
        ad9833_send(ad_settings.command_reg);
        _delay_us(50);
        sbi(ddsPort,portno);
        _delay_us(50);
}

/**
 * sets the desired ad9833 internal phase register to a value that
 * produces the desired phase.
 *
 * \param reg the desired phase register to be manipulated, either 0 or 1
 * \param phase the desired phase
 */
void ad9833_set_phase(uint8_t portno, uint8_t reg, double phase) {
        uint16_t registerTemp;    //probably should be renamed...
        if (reg == 1) registerTemp = AD_PHASE1;
        else registerTemp = AD_PHASE0;

        ad_settings.phase[reg] = phase;

        cbi(ddsPort,portno);
        _delay_us(5);
        ad9833_send(registerTemp | AD_PHASE_CALC(ad_settings.phase[reg]));
        _delay_us(5);
        sbi(ddsPort,portno);
}

/**
 * Selects which frequency register is used to generate the output.
 * Also used to select FSK.
 * \param phase_out possible values:
 *      - 0 = use phase register 0
 *      - 1 = use phase register 1
 *      - 2 = PSK
 */
void ad9833_set_freq_out(uint8_t portno, uint8_t freq_out) {
        ad_settings.freq_out = freq_out;
        switch (freq_out) {
                case 0:
                        ad_settings.command_reg &= ~(1 << AD_FSELECT);
                        break;
                case 1:
                        ad_settings.command_reg |= (1 << AD_FSELECT);
                        break;
        }

        cbi(ddsPort,portno);
        _delay_us(5);
        ad9833_send(ad_settings.command_reg);
        _delay_us(5);
        sbi(ddsPort,portno);
}

/**
 * Selects which phase register is used to generate the output
 * Also used to select PSK
 * \param phase_out possible values:
 *  - 0 = use phase register 0
 *  - 1 = use phase register 1
 *  - 2 = PSK
 */
void ad9833_set_phase_out(uint8_t portno, uint8_t phase_out) {
        ad_settings.phase_out = phase_out;
        switch (phase_out) {
                case 0:
                        ad_settings.command_reg &= ~(1 << AD_PSELECT);
                        break;
                case 1:
                        ad_settings.command_reg |= (1 << AD_PSELECT);
                        break;
        }

        cbi(ddsPort,portno);
        _delay_us(5);
        ad9833_send(ad_settings.command_reg);
        _delay_us(5);
        sbi(ddsPort,portno);
}

/**
 * returns the previously set phase output mode.
 */
uint8_t ad9833_get_phase_out(void) {
        return ad_settings.phase_out;
}

/**
 * sets the desired ad9833 internal frequency register to a value that
 * produces the desired frequency.
 *
 * \param reg the desired frequency register to be manipulated, either 0 or 1
 * \param freq the desired frequency
 */
void ad9833_set_frequency(uint8_t portno, uint8_t reg, double freq) {
        uint32_t freq_reg;
        uint16_t reg_reg;    //probably should be renamed...
        freq_reg = AD_FREQ_CALC(freq);
        ad_settings.freq[reg] = freq;

        if (reg == 1) reg_reg = AD_FREQ1;
        else reg_reg = AD_FREQ0;

        cbi(ddsPort,portno);
        _delay_us(5);
        ad9833_send((1 << AD_B28) | ad_settings.command_reg);
        ad9833_send(reg_reg | (0x3FFF & (uint16_t) (freq_reg >> 2)));
        ad9833_send(reg_reg | (0x3FFF & (uint16_t) (freq_reg >> 16)));
        _delay_us(5);
        sbi(ddsPort,portno);
}
