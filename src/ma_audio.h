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

void ma_audio_init(void);
void ma_audio_process(void);
uint16_t* ma_audio_spectrum(void);

extern uint16_t adc_maxS;
extern uint16_t adc_minS;
extern uint16_t last_capture;

#endif /* SRC_MA_AUDIO_H_ */
