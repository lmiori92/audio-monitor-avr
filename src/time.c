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
 * @file time.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Time keeping routines
 */

#include "stdbool.h"

/* avr libs */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define TIMER_0_PRESCALER_8     (1 << CS01)
#define TIMER_0_PRESCALER_64    ((1 << CS01) | (1 << CS00))
#define TIMER_0_PRESCALER_256   (1 << CS02)

/*
 * Clock is 12MHz
 * Prescaler is 8
 * So, a complete overflow happens every: 170us
 * The target resolution is 100us but timer0 on atmega8
 * does not have a comparator interrupt. Hence, a workaround
 * is to manually increment the counter register when the timer
 * overflows. The value is simply the following:
 * number to add = 0xFF - required timer0 clocks
 * (in our case -> 255 - 150 -> 105
 *
 * */

uint32_t g_timestamp;

/* initialize timer, interrupt and variable.
 * Assumes that interrupts are disabled while intializing
 */
void timer_init()
{

    /* set up timer with prescaler */
    TCCR0 = TIMER_0_PRESCALER_8;

    /* initialize counter */
    TCNT0 = 105;

    /* enable overflow interrupt */
    TIMSK |= (1 << TOIE0);

}
/* Timer comparator interrupt routine */
ISR(TIMER0_OVF_vect)
{
    g_timestamp += 100;   	/* 100us */
    TCNT0 		+= 105;		/* Advance internal counter */
}

