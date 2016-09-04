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
 * @file ma_util.h
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Header for the miscellaneous utility routines
 */

#ifndef __MA_UTIL__
#define __MA_UTIL__

#include "stdbool.h"

/** This structure describes a menu entry */
typedef struct
{
    uint8_t brightness;     /**< Display brightness */
    uint8_t audio_source;   /**< Last used audio source */
    uint8_t meter_type;     /**< Preferred meter type */
} t_persistent;

typedef struct
{
    bool     input_old;         /**< Last input state */
    uint32_t rising_timeout;    /**< Rising time */
    uint32_t falling_timeout;   /**< Falling time */
    uint32_t timestamp;         /**< Current timestamp */

} t_debounce;

typedef struct
{
    uint16_t output;
    uint16_t alpha;

    uint32_t output_last;
} t_low_pass_filter;

#define SOURCE_MAX    4         /**< Number of audio sources */

/* EEPROM */
void read_from_persistent(t_persistent* persistent);
void write_to_persistent(t_persistent* persistent);

/* Timing */
bool debounce(t_debounce *debounce, bool input, uint32_t timestamp);

/* Source selection */
uint8_t source_select(uint8_t source);

/* Algorithms */
uint32_t usqrt(uint32_t x);
void low_pass_filter(uint16_t input, t_low_pass_filter *filter);

#endif

