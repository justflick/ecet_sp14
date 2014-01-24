

#ifndef _AD9833_H_
#define _AD9833_H_

#define AD_OFF      0
#define AD_TRIANGLE 1
#define AD_SQUARE   2
#define AD_SINE     3

/** \name AD9833 command register bits
 * @{
 */

#define AD_B28     13
#define AD_HLB     12
#define AD_FSELECT 11
#define AD_PSELECT 10
#define AD_RESET   8
#define AD_SLEEP1  7
#define AD_SLEEP12 6
#define AD_OPBITEN 5
#define AD_DIV2    3
#define AD_MODE    1

/** @}*/

/** \name AD9833 register addresses
 * @{
 */
#define AD_FREQ0  (1<<14)
#define AD_FREQ1  (1<<15)
#define AD_PHASE0 (3<<14)
#define AD_PHASE1 ((3<<14)|(1<<13))

/** @}*/

/** \name AD9833 calculation macros
 * @{ */
#define AD_F_MCLK 20e6 ///<Clock speed of the ad9833 reference clock
#define AD_2POW28 268435456 ///<used in calculating output freq
/** Macro that calculates the value for a ad9833 frequency register from a frequency */
#define AD_FREQ_CALC(freq) (uint32_t)(((double)AD_2POW28/(double)AD_F_MCLK*freq)*4)

/** Macro that calculates value for Timer1 output compare from a frequency*/
#define AD_MOD_FREQ_CALC(freq) (F_CPU/(64*(uint32_t)freq))

/** Macro that calculates the value for a ad9833 phase register from a phase in degrees */
#define AD_PHASE_CALC(phase_deg) (uint16_t)((512*phase_deg)/45)
/** @} */

/** Struct that holds all the configuration it's initialized as a global variable
 * in the ad9833.c file */
typedef struct {
        float freq[2];    ///<Holds the frequencies of
        float phase[2];
        float mod_freq;
        uint8_t freq_out;
        uint8_t phase_out;
        uint8_t mode;
        uint16_t command_reg;
} ad9833_settings_t;

void ad9833_init(uint8_t portno);

void ad9833_set_mode(uint8_t portno, uint8_t mode);

void ad9833_set_frequency(uint8_t portno, uint8_t reg, double freq);
double ad9833_get_frequency(uint8_t reg);

void ad9833_set_phase(uint8_t portno, uint8_t reg, double phase);
double ad9833_get_phase(uint8_t reg);

void ad9833_set_freq_out(uint8_t portno, uint8_t freq_out);
uint8_t ad9833_get_freq_out(void);

void ad9833_set_phase_out(uint8_t portno, uint8_t phase_out);
uint8_t ad9833_get_phase_out(void);

#endif

/** @} */
