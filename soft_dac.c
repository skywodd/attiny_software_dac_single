/*
 * See header file for details
 *
 *  This program is free software: you can redistribute it and/or modify\n
 *  it under the terms of the GNU General Public License as published by\n
 *  the Free Software Foundation, either version 3 of the License, or\n
 *  (at your option) any later version.\n
 * 
 *  This program is distributed in the hope that it will be useful,\n
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n
 *  GNU General Public License for more details.\n
 * 
 *  You should have received a copy of the GNU General Public License\n
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.\n
 */

/* Includes */
#include <avr/interrupt.h> // For ISR
#include <avr/io.h>        // For registers

void soft_dac_initialize( void ) {

  /* Disallow interrupts */
  cli();

  /* Initialize OC0B pin (PB1) */
  DDRB |= (1 << PIN1);   // PB1 as output
  PORTB &= ~(1 << PIN1); // PB1 at low state

  /* Initialize Timer 0 (for fast PWM) */
  // OC0A disconnected
  // OC0B connected to PB1 w/ "Clear on compare match, set at bottom"
  // Fast PWM mode enable
  TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (1 << COM0B1) | (0 << COM0B0) | (1 << WGM01) | (1 << WGM00);
  // PWM mode
  // Fast PWM mode enable (top at 0xFF)
  // Prescaler CLK_IO/1
  TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (0 << CS02) | (0 << CS01) | (1 << CS00);
  TCNT0 = 0; // Reset Timer 0 counter
  OCR0A = 0; // OCR0A not used
  OCR0B = 0; // Set defaut output value to 0

  /* Initialize Timer 1 (for sampling) */
  // CTC mode disable
  // No PWM output
  // No output on OC1A pin
  // Set defaut output prescaler to 0 (timer off)
  TCCR1 = (1 << CTC1) | (0 << PWM1A) | (0 << COM1A1) | (0 << COM1A0) | (0 << CS13) | (0 << CS12) | (0 << CS11) | (0 << CS10);
  // No PMW output
  // No output on OC1B pin
  // No force compare mode
  // No prescaler reset
  GTCCR = (0 << PWM1B) | (0 << COM1B1) | (0 << COM1B0) | (0 << FOC1B) | (0 << FOC1A) | (0 << PSR1) | (0 << PSR0);
  TCNT1 = 0; // Reset Timer 1 counter
  OCR1A = 0; // Set default compare value to 0
  OCR1B = 0; // OCR1B not used
  OCR1C = 0; // OCR1C not used

  /* Initialize interrupts register */
  // Timer 1 : Interrupt on compare match A
  // Timer 1 : No interrupt on compare match B
  // Timer 1 : No interrupt on counter overflow
  // Timer 0 : No interrupt on compare match A and B
  // Timer 0 : No interrupt on counter overflow
  TIMSK = (1 << OCIE1A) | (0 << OCIE1B) | (0 << OCIE0A) | (0 << OCIE0B) | (0 << TOIE1) | (0 << TOIE0);

  /* Initialize global PLL clock */
  // Low speed mode disable
  // PLL disable
  PLLCSR = (0 << LSM) | (0 << PCKE) | (1 << PLLE);
}

void soft_dac_start( void ) {

  /* Start PWM timer */
  TCNT0 = 0; // Reset Timer 0 counter
  TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00); // Timer 0 : Prescaler CLK_IO/1

  /* Start sampling timer */
  TCNT1 = 0; // Reset Timer 1 counter
  TIMSK |= (1 << OCIE1A);

  /* Allow interrupts */
  sei();
}

void soft_dac_stop( void ) {

  /* Disallow interrupts */
  cli();

  /* Stop PWM timer */
  TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // Timer 0 disable

  /* Stop sampling timer */
  TIMSK &= ~(1 << OCIE1A); // Timer 1 disable

  /* Ground OC0B pin */
  PORTB &= ~(1 << PIN1); // PB1 at low state
}

void soft_dac_sampling_frequency( uint32_t frequency ) {

  /* Usage variable */
  uint8_t prescaler; // Computed prescaler bits
  uint16_t ocr; // Computed OCR1A offset

  /* Compute prescaler and OCR1A offset values */
  for(prescaler = 1; prescaler < 16; ++prescaler) {
    ocr = F_CPU / frequency / ((uint16_t)1 << prescaler) - 1;
    if(ocr <= 255) break;
  }
  if(ocr > 255) ocr = 255;

  OCR1A = ocr & 0xFF; // Set compare value
  TCCR1 = (TCCR1 & ~((1 << CS13) | (1 << CS12) | (1 << CS11) | (1 << CS10))) | prescaler;
}