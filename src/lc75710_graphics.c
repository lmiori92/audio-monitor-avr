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

void display_string_len(char* string, uint8_t len)
{

    uint8_t addr = 0;

    do
    {
        lc75710_dcram_write(len - 1 - addr++, *string);
        string++;
    }
    while (addr < len);

}

void display_string(char* string)
{
    uint8_t len = 0;

    len = strlen(string);

    lc75710_set_ac_address(0, 0);

    display_string_len(string, len);
}

void display_string_center(char* string)
{

    uint8_t len = 0;

    len = strlen(string);

    lc75710_set_ac_address(LC75710_DRAM_SIZE - ((LC75710_DIGITS - len) / 2), 0);

    display_string_len(string, len);

}

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

/* Load vertical bars in the CGRAM of the chip */
void display_load_bars_vert()
{

    uint8_t i = 0;
    uint64_t c = 0;

    for (i = 0; i < 7; i++)
    {
        c |= (uint64_t)0x1F << (30 - (i*5));
        lc75710_cgram_write(i, c);
    }

}

/* Load horizontal bars in the CGRAM of the chip */
void display_load_bars_horiz()
{

    uint8_t i = 0;
    uint64_t c = 0;

    for (i = 0; i < 7; i++)
    {
        c |= (uint64_t)((uint64_t)0x1 << (4 - i));
        c |= (uint64_t)((uint64_t)0x1 << (9 - i));
        c |= (uint64_t)((uint64_t)0x1 << (14 - i));
        c |= (uint64_t)((uint64_t)0x1 << (19 - i));
        c |= (uint64_t)((uint64_t)0x1 << (24 - i));
        c |= (uint64_t)((uint64_t)0x1 << (29 - i));
        c |= (uint64_t)((uint64_t)0x1 << (34 - i));
        lc75710_cgram_write(i, c);
    }

}

void display_load_vumeter_bars_in_ram()
{

    uint8_t i = 0;
    uint64_t c = 0;

    c = (uint64_t) 0xCC3;
    for (i = 0; i < 7; i++)
    {
        lc75710_cgram_write(i, c);//(uint64_t)0x7FFFFFFFFL << (64-35)) << (i * 5));
    }

}

void display_show_horizontal_bar(uint8_t level)
{

    uint8_t i = 0;

    display_clear();
    lc75710_set_ac_address(0, 0);

    /* Full bars */
    for (i = 0; i < level / 5; i++)
    {
        lc75710_dcram_write(i, 4);
    }

    lc75710_dcram_write(i, level%5-1);

}


void display_show_horiz_bars(uint8_t bar, uint8_t level)
{
    // TODO we want to saturate here and not % ;=)
    if (bar > 9) bar = 9;   /* Saturate */
    if (level > 6) level = 6;
    lc75710_dcram_write(9 - bar, level);
}
