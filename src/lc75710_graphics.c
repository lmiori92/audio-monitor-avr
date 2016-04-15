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
 * @file lc75710_graphics.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Implementation of LC75710 graphics routines
 */

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "lc75710.h"

/**
 *
 * display_string_len
 *
 * @brief Display a string having a specified length
 *
 * @param string    string to display
 * @param len       length of the string
 */
void display_string_len(const char* string, uint8_t len)
{

    uint8_t addr = 0;

    do
    {
        lc75710_dcram_write(len - 1 - addr++, *string);
        string++;
    }
    while (addr < len);

}

/**
 *
 * display_string
 *
 * @brief Display a string
 *
 * @param string zero-terminated string to display
 */
void display_string(const char* string)
{
    uint8_t len = 0;

    len = strlen(string);

    lc75710_set_ac_address(0, 0);

    display_string_len(string, len);
}

/**
 *
 * display_string_center
 *
 * @brief Display a string, center justified
 *
 * @param string zero-terminated string to display
 */
void display_string_center(const char* string)
{

    uint8_t len = 0;

    len = strlen(string);

    lc75710_set_ac_address(LC75710_DRAM_SIZE - ((LC75710_DIGITS - len) / 2), 0);

    display_string_len(string, len);

}

/**
 *
 * display_clear
 *
 * @brief Clear display (i.e. sets RAM to 0x20) AND reset RAM pointer
 *
 */
void display_clear(void)
{

    uint8_t i = 0;

    lc75710_set_ac_address(0, 0);

    for (i = 0; i < LC75710_DIGITS; i++)
    {
        /* Fill with spaces */
        lc75710_dcram_write(i, 0x20);
    }

}

/**
 *
 * display_load_bars_vert
 *
 * @brief Load vertical bars in the CGRAM of the chip
 *
 */
void display_load_bars_vert(void)
{

    uint8_t i = 0;
    uint64_t c = 0;

    for (i = 0; i < 7; i++)
    {
        c |= (uint64_t)0x1F << (30 - (i*5));
        lc75710_cgram_write(i, c);
    }

}

/**
 *
 * display_load_bars_horiz
 *
 * @brief Load horizontal bars in the CGRAM of the chip
 *
 */
void display_load_bars_horiz(bool upper_or_lower)
{

    uint8_t i = 0;
    uint64_t c = 0;

    for (i = 0; i < 5; i++)
    {
        if (upper_or_lower == true)
        {
            c |= (uint64_t)((uint64_t)0x1 << (4 - i));
            c |= (uint64_t)((uint64_t)0x1 << (9 - i));
            c |= (uint64_t)((uint64_t)0x1 << (14 - i));
        }
        else
/*        c |= (uint64_t)((uint64_t)0x1 << (19 - i));   */ /* Split line */
        {
            c |= (uint64_t)((uint64_t)0x1 << (24 - i));
            c |= (uint64_t)((uint64_t)0x1 << (29 - i));
            c |= (uint64_t)((uint64_t)0x1 << (34 - i));
        }

        lc75710_cgram_write(i, c);
    }

}

/**
 *
 * display_load_vumeter_harrows
 *
 * @brief Load VU-meter bars in the CGRAM of the chip
 *
 */
void display_load_vumeter_harrows(void)
{

    /* Arrow-like symbol */

    lc75710_cgram_write(0, (uint64_t) (0xCC3));         /* Right Channel (above) */
    lc75710_cgram_write(1, (uint64_t) 0xCC3 << 20);     /* Left Channel (below)  */
    lc75710_cgram_write(2, (uint64_t) 0xCC3 | (uint64_t) 0xCC3 << 20);   /* R+L Channel */

}

/**
 *
 * display_show_vumeter_harrows
 *
 * @brief Show VU-meter bars on the display
 *
 * \param   left       Left Level 0-10
 * \param   right      Right Level 0-10
 * \param   right_left true: left to right; false: right to left
 *
 */
void display_show_vumeter_harrows(uint8_t left, uint8_t right, bool right_left)
{

    uint8_t i = 0;
    uint8_t c = 0;

    for (i = 1; i <= LC75710_DIGITS; i++)
    {
        if (left >= i && right >= i)
        {
            /* See display_load_vumeter_harrows()  */
            c = 2;
        }
        else if (right >= i)
        {
            /* See display_load_vumeter_harrows()  */
            c = 0;
        }
        else if (left >= i)
        {
            /* See display_load_vumeter_harrows()  */
            c = 1;
        }
        else
        {
            /* Space - Clear */
            c = 0x20;
        }
        lc75710_dcram_write(right_left ? (LC75710_DIGITS - i) : i, c);
    }

}

/**
 *
 * display_show_horizontal_bar
 *
 * @brief Show a horizontal bar accross the whole display
 *
 * @param   level   bar length
 *
 */
void display_show_horizontal_bar(uint8_t level)
{

    uint8_t i = 0;

    lc75710_set_ac_address(0, 0);

    for (i = 0; i < LC75710_DIGITS; i++)
    {
        if (i <= (level / 5))
            lc75710_dcram_write(i, 4);
        else if ((level / 5)+1 == i)
            lc75710_dcram_write(i, level%5-1);
        else
            lc75710_dcram_write(i, 0x20);
    }



}

/**
 *
 * display_show_vertical_bars
 *
 * @brief Show a vertical bar specifying its height and its position
 *
 * @param   bar     character position in the screen
 * @param   level   bar level (intensity)
 *
 */
void display_show_vertical_bars(uint8_t bar, uint8_t level)
{
    if (bar > 9) bar = 9;   /* Saturate */
    if (level > 6) level = 6;
    lc75710_dcram_write(9 - bar, level);
}
