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
 * @file system.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief System level utilities: ISR debugging / MCU power state
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "lc75710_graphics.h"
#include "system.h"

/**
 * ISR(BADISR_vect)
 *
 * @brief This interrupt handler is executed whenever an ISR is fired
 * without a defined ISR routine.
 * It tries to write a string on the display and then blocks.
 */
ISR(BADISR_vect)
{
    display_string("no ISR!");
    for(;;);
}

void system_fatal(char *str)
{
    display_string(str);
    for(;;);
}

/**
 *
 * system_init
 *
 * @brief System init
 *
 */
void system_init(void)
{
//    if(MCUCSR & (1<<PORF )) (PSTR("Power-on reset.\n"));
//    if(MCUCSR & (1<<EXTRF)) (PSTR("External reset!\n"));
//    if(MCUCSR & (1<<BORF )) (PSTR("Brownout reset!\n"));
//    if(MCUCSR & (1<<WDRF )) (PSTR("Watchdog reset!\n"));
    operational.reset_reason = MCUCSR;

    /* Reset state for the next proper detection */
    MCUCSR = 0;
}
