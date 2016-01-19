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
#include "time.h"
#include "ma_util.h"
#include "system.h"

/* Quick and dirty noise debug - TODO remove me or clean the code afterwards */
uint16_t adc_maxS;
uint16_t adc_minS;

static uint16_t last_capture;           /**< Last reading */
static int8_t capture_index;            /**< Buffer index */
static int16_t capture[FFT_N];          /**< Wave capturing buffer */

static complex_t bfly_buff[FFT_N];      /**< FFT buffer */
static uint16_t spektrum[FFT_N/2];      /**< Spectrum output buffer */

/**
 * ISR(ADC_vect)
 *
 * @brief ADC interrupt routine.
 *        Keep it as small as possible to avoid jitter and lower
 *        sampling rate.
 *
 */
ISR(ADC_vect)
{

    if (capture_index >= FFT_N)
    {
        /* Audio sampling complete.
         * quit the ISR without trigger a new conversion */
    }
    else
    {
        /* Save capture in the buffer and apply digital bias */
        capture[capture_index] = 32676U - (ADCL | (ADCH << 8U));
/* HARDWARE NOISE DEBUG
        if (last_capture > adc_maxS) adc_maxS = last_capture;
        if (last_capture < adc_minS) adc_minS = last_capture;
        */

        /* Increment buffer index */
        capture_index++;

        /* Kick-in another conversion */
        /* Set ADSC in ADCSRA (0x7A) to start another ADC conversion */
        ADCSRA |= (1 << ADSC);
    }

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
    adc_maxS = 0;
    adc_minS = 1024;

    /* clear ADLAR in ADMUX (0x7C) to right-adjust the result */
    /* ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits) */
    /* Set REFS1..0 in ADMUX to change reference voltage */
    /* Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog channel */
    ADMUX &= ~((1 << ADLAR) | (1 << REFS1) | (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));

    /* Set the internal 2.56V reference */
    ADMUX |= ((1 << REFS1) | (1 << REFS0));

    /* TODO Set default sampled channel if needed */

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

// TODO work the hann window later, maybe using a lookup table as well
//void hann_window(uint16_t *stream, uint8_t len)
//{
//    for (uint8_t i = 1; i < len; i++) {
//        stream[i] = 0.5f * (1.0f - cos(2.0f * 3.14f * i / (len - 1.0f))) * stream[i];
//    }
//}

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

    if (((ADCSRA >> ADSC) & 0x1) == 0)
    {
        /* Sampling complete */
/*        hann_window(capture, FFT_N);  */
        fft_input(capture, bfly_buff);
        fft_execute(bfly_buff);
        fft_output(bfly_buff, spektrum);

        /* Toggle channel */
        /* TODO iterate through the needed channels:
         * Audio (R+L) has higher priority */
        //uint32_t old_mux = ADMUX;
        //ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
        //ADMUX |= ((old_mux & 0x7) + 1) % 2;

        /* Unset completion flag
         * NOTE: modifying shared variables is valid here,
         * no ISR shall be executed now */
        capture_index = 0;
        ADCSRA |= (1 << ADSC);

    }

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
