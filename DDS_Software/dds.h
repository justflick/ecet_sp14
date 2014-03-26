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

#define AD9833_CLK 20e6
#define AD9833_2POW28 268435456
//#define DDS_FREQ_CALC(freq) (uint32_t)(((double)AD_2POW28/(double)AD_F_MCLK*freq)*4)
//#define AD9833_MOD_FREQ_CALC(freq) (F_CPU/(64*(uint32_t)freq))
#define AD_PHASE_CALC(phase_deg) (uint16_t)((512*phase_deg)/45)

/** @}*/

/** \name AD9833 Waveshape types
 * @{
 */
#define AD9833_OFF      0
#define AD9833_TRIANGLE 1
#define AD9833_SQUARE   2
#define AD9833_SINE     3
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
#define AD9833_B28     13
#define AD9833_HLB     12
#define AD9833_FSELECT 11
#define AD9833_PSELECT 10
#define AD9833_RESET   8
#define AD9833_SLEEP1  7
#define AD9833_SLEEP12 6
#define AD9833_OPBITEN 5
#define AD9833_DIV2    3
#define AD9833_MODE    1
/** @}*/

/** \name Bit macros to select the active
 * freq and phase registers of the ad9833
 * @{
 */
#define AD_FREQ0  (1<<14)
#define AD_FREQ1  (1<<15)
#define AD_PHASE0 (3<<14)
#define AD_PHASE1 ((3<<14)|(1<<13))

//Redeclaration of global variables for linker asist.
extern volatile ddsDevices_t ddsDevices;
extern volatile ad5204_settings_t dac0,dac1;





void ad9833Init(void);//void ad9833_set_mode(ddsDevices_t* DDS_temp);
void analogAdjust(ad5204 *data);
void ad5204SetVal(ad5204_settings_t *ad5204);
void SpiInit(uint8_t clock_polarity, uint8_t clock_phase);
void ad9833_set_frequency(uint32_t freq) ;
void ad9833_set_phase(uint16_t phase);
void ad9833_set_mode(uint8_t mode);
#endif /* DDS_H_ */
