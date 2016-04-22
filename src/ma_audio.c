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
#include "math.h"

#include "ffft.h"
#include "time.h"
#include "ma_util.h"
#include "system.h"
#include "ma_audio.h"

/*#define ADC_NOISE_DEBUG*/

/* Quick noise debug */
#ifdef ADC_NOISE_DEBUG
uint16_t adc_maxS = 0;
uint16_t adc_minS = 0xFFFF;
uint16_t last_captureS = 0;           /**< Last reading */
#endif

static int8_t capture_index = 0U;       /**< Buffer index */
static int16_t capture[FFT_N];          /**< Wave capturing buffer */

static complex_t bfly_buff[FFT_N];      /**< FFT buffer */
static uint16_t spektrum[FFT_N/2];      /**< Spectrum output buffer */

static t_audio_voltage input_level;     /**< Store audio information */

static bool fft_enabled = false;

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
        /* Save capture in the buffer */
#ifdef ADC_NOISE_DEBUG
        last_captureS = (ADCL | (ADCH << 8U));
        capture[capture_index] = last_captureS;
        /* HARDWARE NOISE DEBUG */
        if (last_captureS > adc_maxS) adc_maxS = last_captureS;
        if (last_captureS < adc_minS) adc_minS = last_captureS;
#else
        capture[capture_index] = (ADCL | (ADCH << 8U));
#endif

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

    /* clear ADLAR in ADMUX (0x7C) to right-adjust the result */
    /* ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits) */
    /* Set REFS1..0 in ADMUX to change reference voltage */
    /* Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog channel */
    ADMUX &= ~((1 << ADLAR) | (1 << REFS1) | (1 << REFS0) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));

    /* Set the internal 2.56V reference */
    ADMUX |= ((1 << REFS1) | (1 << REFS0));

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

/* Quick and dirty Hann Window for post-process the FFT spectrum
 * - optimize
 * - clean
 * - ? ;=)*/
static void hann_window(uint16_t *stream, uint8_t len)
{
    for (uint8_t i = 1; i < len; i++)
    {
        stream[i] = 0.5f * (1.0f - cos((double)(2.0f * 3.14f * i / (len - 1.0f)))) * stream[i];
    }
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

    uint8_t old_mux;
    uint32_t rms = 0;   /* 32 bits because of the power calculations */
    uint16_t tmp = 0;
    uint8_t i = 0;

    if (((ADCSRA >> ADSC) & 0x1) == 0)
    {
        /* Sampling complete */
        if (fft_enabled == true)
        {
            fft_input(capture, bfly_buff);
            fft_execute(bfly_buff);
            fft_output(bfly_buff, spektrum);
            hann_window(spektrum, FFT_N/2);
        }

        /* Toggle channel */
        /* iterate through the needed channels:
         * Audio (R+L) has higher priority */
        old_mux = ADMUX & 0x7U;

        /* zero current mux option */
        ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
        /* set the new mux */
        ADMUX |= (old_mux + 1U) % 2U;

        /* VU-METER testing */
        for(i = 0; i < FFT_N; i++)
        {
            if (capture[i] >= 512)
            {
                tmp = (capture[i] - 512);
            }
            else
            {
                tmp = (512 - capture[i]);
            }
            rms += tmp * tmp;
        }

        if (old_mux == 0U)
        {
            /* Left Channel */
            rms = input_level.left + ((sqrt((double)rms / FFT_N) - input_level.left) * 350 / 1000);
            input_level.left = rms;
        }
        else if(old_mux == 1U)
        {
            /* Left Right */
            rms = input_level.right + ((sqrt((double)rms / FFT_N) - input_level.right) * 350 / 1000);
            input_level.right = rms;
        }
        else
        {
            /* Not handled */
        }

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
 * @param   buckets     the pointer to the variable holding FFT size
 *
 * @return  the audio spectrum (FFT output)
 */
uint16_t* ma_audio_spectrum(uint8_t *buckets)
{
    *buckets = FFT_N;
    return spektrum;
}

/**
 *
 * ma_audio_last_capture
 *
 * @brief Getter function for the last ADC capture
 *
 * @param   last_capture    pointer to the variable to store the last capture value
 * @param   adc_min         pointer to the variable to store the minimum value
 * @param   adc_max         pointer to the variable to store the maximum value
 * @return the value for the last capture
 *
 */
void ma_audio_last_capture(uint16_t *last_capture, uint16_t *adc_min, uint16_t *adc_max)
{
#ifdef ADC_NOISE_DEBUG
    *last_capture = last_captureS;
    *adc_min = adc_minS;
    *adc_max = adc_maxS;
#endif
}

void ma_audio_last_reset(void)
{
#ifdef ADC_NOISE_DEBUG
    last_captureS = 0;
    adc_minS = 0xFFFF;
    adc_maxS = 0;
#endif
}

t_audio_voltage* ma_audio_last_levels(void)
{
    return &input_level;
}

void ma_audio_fft_process(bool flag)
{
    fft_enabled = flag;
}
