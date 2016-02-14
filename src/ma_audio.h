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
 * @file ma_audio.h
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Header file for the audio routines.
 */

#ifndef SRC_MA_AUDIO_H_
#define SRC_MA_AUDIO_H_

typedef struct _audio_voltage
{
    uint16_t left;
    uint16_t right;
} t_audio_voltage;

void ma_audio_init(void);
void ma_audio_process(void);
uint16_t* ma_audio_spectrum(uint8_t *buckets);
t_audio_voltage* ma_audio_last_levels(void);

void ma_audio_last_capture(uint16_t *last_capture, uint16_t *adc_min, uint16_t *adc_max);

void ma_audio_last_reset(void);

#endif /* SRC_MA_AUDIO_H_ */
