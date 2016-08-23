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

#include "deasplay/driver/LC75710/lc75710.h"
#include "deasplay/deasplay.h"    /* display API */

/**
 *
 * display_string_center
 *
 * @brief Display a string, center justified
 *
 * @param string zero-terminated string to display
 */
void display_string_center(char* string)
{

    uint8_t len = 0;

    len = strlen(string);

    display_set_cursor(0, ((LC75710_DIGITS - len) / 2));

    display_write_string(string);

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
            c |= (uint64_t)((uint64_t)0x1 << (i));
            c |= (uint64_t)((uint64_t)0x1 << (i + 5));
            c |= (uint64_t)((uint64_t)0x1 << (i + 10));
        }
        else
/*        c |= (uint64_t)((uint64_t)0x1 << (19 - i));   */ /* Split line */
        {
            c |= (uint64_t)((uint64_t)0x1 << (i + 20));
            c |= (uint64_t)((uint64_t)0x1 << (i + 25));
            c |= (uint64_t)((uint64_t)0x1 << (i + 30));
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
 * @brief Show VU-meter bars on the display, from the position that has been set beforehand
 *
 * \param   left       Left Level 0-10
 * \param   right      Right Level 0-10
 *
 */
void display_show_vumeter_harrows(uint8_t left, uint8_t right)
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

        display_write_char(c);
    }

}

/**
 *
 * display_show_horizontal_bar
 *
 * @brief Show a horizontal bar accross the display,
 *        from the position that isset beforehand (usually 0,0)
 *
 * @param   level   bar length, spanning from 0 to 50 units
 *
 */
void display_show_horizontal_bar(uint8_t level)
{

    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t chr = 0x20U;

    while (level > 0U)
    {
        if (level >= 5U)
        {
            display_write_char(4);
            level -= 5U;
        }
        else
        {
            display_write_char(4U - level);
            level = 0U;
        }
    }



/*
    for (i = 0; i < LC75710_DIGITS * 5U; i++)
    {

        if (j == 0)
        {
            chr = 0x20;
        }
        else if ((level < i) && (level > 0U))
        {
            ///* reached the top, always a space from now on
        }
        else
        {
            chr = 4U - j;
        }
        */
/*
        j++;

        if (j >= 5U)
        {
            display_write_char(chr);
            // write char and then go to next
            j = 0U;
            chr = 0x20;
        }
        */

        /*
        if (level > 0U)
        {
            for (j = 0U; j < 5U; j++)
            {
                if (level < j)
                {
                    break;
                }
            }
            display_write_char(j);
            level -= j;
        }
        else
        {
            display_write_char(0x20U);
        }
        */


}

/**
 *
 * display_show_vertical_bar
 *
 * @brief Show a vertical bar specifying its height. Position has to be
 *        set beforehand using the appropriate screen directives/driver.
 *
 * @param   level   bar level (intensity)
 *
 */
void display_show_vertical_bar(uint8_t level)
{
    if (level > 6) level = 6;
    display_write_char(level);
}
