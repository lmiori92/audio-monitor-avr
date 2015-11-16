/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Lorenzo Miori (C) 2015 [ 3M4|L: memoryS60<at>gmail.com ]

    Version History
        * 1.0 initial

*/

/**
 * @file ma_audio.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief The audio input processing section. It includes ADC handling and related computation.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ffft.h"

//#define ADCH0   0
//#define ADCH1   1

#include "avr/delay.h"
#include "time.h"
#include "ma_util.h"
#include "system.h"

static uint16_t last_capture;           /**< Last reading */
static int8_t capture_index;            /**< Buffer index */
static int16_t capture[FFT_N];          /**< Wave capturing buffer */

static complex_t bfly_buff[FFT_N];      /**< FFT buffer */
static uint16_t spektrum[FFT_N/2];      /**< Spectrum output buffer */

/**
 * ISR(ADC_vect)
 *
 * @brief ADC interrupt routine.
 *        Keep it as small as possible to avoid jitter and delays.
 *
 * The strategy
 *
 */
ISR(ADC_vect)
{

//    uint8_t adcChan;

    /* ADC channel that has completed the reading */
//    adcChan = ADMUX & 0xF;
    last_capture = ADCL | (ADCH << 8);

    /* Save capture in the buffer */
    capture[capture_index] = last_capture - 32768;

    /* Increment buffer index */
    capture_index++;

    /* Toggle the other channel */
//  ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
//  ADMUX |= (adcChan == ADCH0) ? ADCH1 : ADCH0;

    /* Set ADSC in ADCSRA (0x7A) to start another ADC conversion */
    capture_index %= FFT_N;

    /* Change channel every ~20ms to sample low priority stuff */

    /* Kick-in another conversion */
    ADCSRA |= (1 << ADSC);

    operational.adc_samples++;

}

/**
 *
 * ma_audio_init
 *
 * @brief ADC initialization, assuming the interrupts are disabled.
 *
 */
void ma_audio_init(void)
{

    capture_index = 0;
    operational.adc_samples = 0;

    /* clear ADLAR in ADMUX (0x7C) to right-adjust the result */
    /* ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits) */
    /* Set REFS1..0 in ADMUX to change reference voltage */
    /* Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog channel */
    ADMUX &= ~((1 << ADLAR) | (1 << REFS1) | (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));

    /* Set ADEN in ADCSRA (0x7A) to enable the ADC.
       Note, this instruction takes 12 ADC clocks to execute
     */
    ADCSRA |= (1 << ADEN);

    /* Disable Free Running Mode (we want to change channel in the ISR) */
    ADCSRA &= ~(1 << ADFR);

    /* Set the Prescaler to 32 */
    ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0) |
              /* Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt. */
              (1 << ADIE) |
              /* Kickstart the process */
              (1 << ADSC);

}

/**
 *
 * ma_audio_process
 *
 * @brief This function shall be periodically called
 * on the audio buffer to compute FFT / VU-meter
 *
 */
void ma_audio_process(void)
{
    fft_input(capture, bfly_buff);
    fft_execute(bfly_buff);
    fft_output(bfly_buff, spektrum);
}

/**
 *
 * ma_audio_spectrum
 *
 * @brief Getter function for the audio spectrum
 *
 * @return  the audio spectrum (FFT output)
 */
uint16_t* ma_audio_spectrum(void)
{
    return spektrum;
}

/**
 *
 * ma_audio_last_capture
 *
 * @brief Getter function for the last ADC capture
 *
 * @return the value for the last capture
 *
 */
uint16_t ma_audio_last_capture(void)
{
    return last_capture;
}
