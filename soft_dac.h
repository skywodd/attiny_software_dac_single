/**
 * @file soft_dac.h
 * @mainpage
 * @brief Software DAC functions bundle for Attiny 25 / 45 / 85
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This library is designed to create a software DAC with a fast PWM output.\n
 * \n
 * Please report bug to <skywodd at gmail.com>
 *
 * @section licence_sec Licence
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 * \n
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 * \n
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 *
 * @section changelog_sec Changelog history
 * - 08/06/2012 : First version
 * - 09/06/2012 : Replace cascaded if statements by a more readable for loop
 * - 01/09/2012 : Make some fix to the doxygen documentation and typo
 */

#ifndef _H_SOFT_DAC_
#define _H_SOFT_DAC_

#if !defined(__AVR_ATtiny25__) && !defined(__AVR_ATtiny45__) && !defined(__AVR_ATtiny85__)
#error "Software DAC is written for ATtiny25, Attiny45 and Attiny85 only !"
#endif

#include <avr/io.h>

/**
 * Initialize software DAC
 */
void soft_dac_initialize( void );

/**
 * Start software DAC
 */
void soft_dac_start( void );

/**
 * Stop software DAC
 */
void soft_dac_stop( void );

/**
 * Reset sampling timer
 */
inline void soft_dac_sampling_reset( void ) {
  TCNT1 = 0; // Reset Timer 1 counter
}

/**
 * Set output value of software DAC
 *
 * @param value Analog value between 0 and 255
 */
inline void soft_dac_output( uint8_t value ) {
  OCR0B = value; // Set value (double buffered in Fast PWM mode to avoid glitch)
}

/**
 * Set sampling frequency of software DAC
 *
 * @note
 * To handle the sampling ISR add this snippet to your main code :
 * ISR(TIMER1_COMPA_vect) {
 *   // Add this ISR to your code
 * }
 *
 * @param frequency Target frequency in Hert
 */
void soft_dac_sampling_frequency( uint32_t frequency );

#endif // _H_SOFT_DAC_