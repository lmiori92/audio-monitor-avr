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
 * @file ma_util.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Miscellaneous utility routines
 */

#include "stdint.h"
#include "stddef.h"
#include "ma_util.h"

#include <avr/eeprom.h>

/**
* read_from_persistent
*
* @brief Read persistent variables from the non volatile storage
* @param persistent the persistent variables to write to
*/
void read_from_persistent(t_persistent* persistent)
{
    uint8_t i = 0;
    persistent->brightness = eeprom_read_byte((const uint8_t*)i++);
    persistent->audio_source = eeprom_read_byte((const uint8_t*)i++);
    persistent->meter_type = eeprom_read_byte((const uint8_t*)i++);
}

/**
* write_to_persistent
*
* @brief Write persistent variables to the non volatile storage
* @param persistent the persistent variables to read from
*/
void write_to_persistent(t_persistent* persistent)
{
    //eeprom_write_block((void*)5, persistent, sizeof(t_persistent));
    uint8_t i = 0;
    eeprom_write_byte((uint8_t*)i++, persistent->brightness);
    eeprom_write_byte((uint8_t*)i++, persistent->audio_source);
    eeprom_write_byte((uint8_t*)i++, persistent->meter_type);
}

bool debounce(t_debounce *debounce, bool input, uint32_t timestamp)
{
    if (debounce == NULL)
    {
        /* Sorry (and screw you as well), NULL pointer! */
        return input;
    }
    else
    {
        if (input == true && input != debounce->input_old)
        {
            /* input rising edge detected */
            if (debounce->timestamp == 0U)
            {
               /* input has changed. Store the current timestamp. */
                debounce->timestamp = timestamp;
            }
            else
            {
                /* already running */
            }

            if (timestamp - debounce->timestamp >= debounce->rising_timeout)
            {
                return input;
            }
            else
            {
                /* no timeout yet */
            }

        }
        else if (input == false && input != debounce->input_old)
        {
            /* input falling edge detected */
            if (debounce->timestamp == 0U)
            {
               /* input has changed. Store the current timestamp. */
                debounce->timestamp = timestamp;
            }
            else
            {
                /* already running */
            }

            if (timestamp - debounce->timestamp >= debounce->rising_timeout)
            {
                return input;
            }
            else
            {
                /* no timeout yet */
            }
        }
        else
        {
            debounce->timestamp = 0U;
            return debounce->input_old;
        }

    }

    return debounce->input_old;

}

uint8_t source_select(uint8_t source)
{

    uint8_t outputs;

    switch(source)
    {
        case 0:
            outputs = 1 << 2;
            break;
        case 1:
            outputs = 1 << 0;
            break;
        case 2:
            outputs = 1 << 1;
            break;
        case 3:
            outputs = (1 << 2) | (1 << 1) | (1 << 0);
            break;
        default:
            outputs = 0;
            break;
    }

    return outputs;

}
// additional change: types are "ported" to stdint portable types
#define BITSPERLONG 32  // originally was 32: with 16, we get the approximate

#define TOP2BITS(x) ((x & (3L << (BITSPERLONG-2))) >> (BITSPERLONG-2))


/* usqrt:
    ENTRY x: unsigned long
    EXIT  returns floor(sqrt(x) * pow(2, BITSPERLONG/2))

    Since the square root never uses more than half the bits
    of the input, we use the other half of the bits to contain
    extra bits of precision after the binary point.

    EXAMPLE
        suppose BITSPERLONG = 32
        then    usqrt(144) = 786432 = 12 * 65536
                usqrt(32) = 370727 = 5.66 * 65536

    NOTES
        (1) change BITSPERLONG to BITSPERLONG/2 if you do not want
            the answer scaled.  Indeed, if you want n bits of
            precision after the binary point, use BITSPERLONG/2+n.
            The code assumes that BITSPERLONG is even.
        (2) This is really better off being written in assembly.
            The line marked below is really a "arithmetic shift left"
            on the double-long value with r in the upper half
            and x in the lower half.  This operation is typically
            expressible in only one or two assembly instructions.
        (3) Unrolling this loop is probably not a bad idea.

    ALGORITHM
        The calculations are the base-two analogue of the square
        root algorithm we all learned in grammar school.  Since we're
        in base 2, there is only one nontrivial trial multiplier.

        Notice that absolutely no multiplications or divisions are performed.
        This means it'll be fast on a wide range of processors.
*/

uint32_t usqrt(uint32_t x)
{
    uint32_t a = 0L;                   /* accumulator      */
    uint32_t r = 0L;                   /* remainder        */
    uint32_t e = 0L;                   /* trial product    */

      uint8_t i;

      for (i = 0; i < BITSPERLONG/2; i++)   /* NOTE 1 */
      {
            r = (r << 2) + TOP2BITS(x); x <<= 2; /* NOTE 2 */
            a <<= 1;
            e = (a << 1) + 1;
            if (r >= e)
            {
                  r -= e;
                  a++;
            }
      }

      return a;
}

void low_pass_filter(uint16_t input, t_low_pass_filter *filter)
{
    uint32_t tmp;

    /* weigh the previous output */
    tmp = filter->alpha;
    tmp *= filter->output_last;
    /* scale the input value and add to it */
    tmp += 1000U * (uint32_t)input;
    tmp /= (filter->alpha + 1U);
    /* save last input value */
    filter->output_last = tmp;

    /* compute the scaled result */
    filter->output = (uint16_t)(filter->output_last / 1000U);
}
