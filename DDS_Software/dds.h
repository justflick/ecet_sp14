/*
 * dds.h
 *
 *  Created on: Jan 31, 2014
 *      Author: Konecranes
 */

#ifndef DDS_H_
#define DDS_H_

#include "main.h"
#include "serial.h"
//#include <avr/sfr_defs.h>
//#include <avr/common.h>

/** \name AD9833 component parameters
 * @{
 */

#define DDS_CLK 20e6
#define DDS_2POW28 268435456
//#define DDS_FREQ_CALC(freq) (uint32_t)(((double)AD_2POW28/(double)AD_F_MCLK*freq)*4)
#define DDS_MOD_FREQ_CALC(freq) (F_CPU/(64*(uint32_t)freq))
#define DDS_PHASE_CALC(phase_deg) (uint16_t)((512*phase_deg)/45)

/** @}*/

/** \name AD9833 Waveshape types
 * @{
 */
#define DDS_OFF      0
#define DDS_TRIANGLE 1
#define DDS_SQUARE   2
#define DDS_SINE     3
/** @}*/



#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SS       DDB2
#define DD_SCK      DDB5



/** \name AD9833 command register bits
 * @{
 */
#define DDS_B28     13
#define DDS_HLB     12
#define DDS_FSELECT 11
#define DDS_PSELECT 10
#define DDS_RESET   8
#define DDS_SLEEP1  7
#define DDS_SLEEP12 6
#define DDS_OPBITEN 5
#define DDS_DIV2    3
#define DDS_MODE    1
/** @}*/

/** \name Bit macros to select the active
 * freq and phase registers of the ad9833
 * @{
 */
#define AD_FREQ0  (1<<14)
#define AD_FREQ1  (1<<15)
#define AD_PHASE0 (3<<14)
#define AD_PHASE1 ((3<<14)|(1<<13))







void ad9833Init(ad9833_settings_t *device);//void ad9833_set_mode(ad9833_settings_t* DDS_temp);
void analogAdjust(ad5204 *data);
void ad9833_set_frequency(ad9833_settings_t *device) ;
double ad9833_get_frequency(uint8_t reg);
void ad9833_set_phase(ad9833_settings_t *device, uint32_t phase);
double ad9833_get_phase(uint8_t reg);
void ad9833_set_freq_out(uint8_t freq_out);
uint8_t ad9833_get_freq_out(void);
void ad9833_set_phase_out(uint8_t phase_out);
uint8_t ad9833_get_phase_out(void);

#endif /* DDS_H_ */
