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
 * @file lc75710_graphics.h
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Header of LC75710 graphics routines
 */

#ifndef SRC_LC75710_GRAPHICS_H_
#define SRC_LC75710_GRAPHICS_H_

#include "stdint.h"
#include "stdbool.h"

#define VUMETER_HARROWS_R   00U
#define VUMETER_HARROWS_L   20U

void display_string_len(const char* string, uint8_t len);
void display_string(const char* string);
void display_string_center(const char* string);
void display_clear(void);
void display_load_bars_vert(void);
void display_load_bars_horiz(bool upper_or_lower);

void display_load_vumeter_harrows(void);
void display_show_vumeter_harrows(uint8_t left, uint8_t right, bool right_left);

void display_show_horizontal_bar(uint8_t level);
void display_show_vertical_bars(uint8_t bar, uint8_t level);

#endif /* SRC_LC75710_GRAPHICS_H_ */
