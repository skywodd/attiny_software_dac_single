/**
 * @file main.c
 * @brief Software DAC example for Attiny 25 / 45 / 85
 * @author SkyWodd
 * @version 1.0
 * @see http://skyduino.wordpress.com/
 *
 * @section intro_sec Introduction
 * This is a example for the ATtiny SoftwareDAC library.\n
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
 */
 
/* Includes */
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include "soft_dac.h"

/* Wavetable */
//#define USE_MUSIC_SAMPLE

#ifdef USE_MUSIC_SAMPLE
#include "music.h"
#else
#include "wavetables.h"

const uint16_t pcm_length = 361;
volatile uint16_t pcm_samples = (uint16_t)wave_sinus;
#endif

/* Wavetble iterator */
volatile uint16_t counter = 0;

/**
 * Sampling ISR
 */
ISR(TIMER1_COMPA_vect) {

  /* Sample frequency debuging */
  //PORTB ^= (1 << PIN0);

  /* output current sample */
  soft_dac_output(pgm_read_byte(pcm_samples + counter));
  
  /* Jump to next sample (or loop back to first sample) */
  if(++counter >= pcm_length) counter = 0;
  
  /* Re-arm sampling timer */
  soft_dac_sampling_reset();
}

/**
 * Programm entry point
 */
int main( void ) {

  /* Initialize pin PB0 (for debug) */
  DDRB |= (1 << PIN0);   // PB0 as output
  PORTB &= ~(1 << PIN0); // PB0 at low state
  
  /* Debug Hello */
  uint8_t i;
  for(i = 0; i < 5; ++i) {
    PORTB ^= (1 << PIN0);
    _delay_ms(200);
  }

  /* Initialize software DAC */
  soft_dac_initialize();
  
  /* Set sampling frequency */
  soft_dac_sampling_frequency(8000UL);
  
  /* Start software ADC */
  soft_dac_start();
  
  /* Infinite loop */
  for(;;) {
#ifndef USE_MUSIC_SAMPLE
    PORTB ^= (1 << PIN0);
	pcm_samples = (uint16_t)wave_sinus;
	_delay_ms(2000);
	
	PORTB ^= (1 << PIN0);
    pcm_samples = (uint16_t)wave_triangle;
	_delay_ms(2000);
	
	PORTB ^= (1 << PIN0);
    pcm_samples = (uint16_t)wave_scare;
	_delay_ms(2000);
	
	PORTB ^= (1 << PIN0);
    pcm_samples = (uint16_t)wave_sawtooth;
	_delay_ms(2000);
	
	PORTB ^= (1 << PIN0);
    pcm_samples = (uint16_t)wave_noise;
	_delay_ms(2000);
#endif
  }
}