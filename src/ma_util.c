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
    //eeprom_read_block(persistent, (void*)5, sizeof(t_persistent));
    persistent->brightness = eeprom_read_byte((const uint8_t*)5);
    persistent->audio_source = eeprom_read_byte((const uint8_t*)6);
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
    eeprom_write_byte((uint8_t*)5, persistent->brightness);
    eeprom_write_byte((uint8_t*)6, persistent->audio_source);
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
            outputs = 0;
            break;
        case 1:
            outputs = 1 << 0;
            break;
        case 2:
            outputs = 1 << 1;
            break;
        case 3:
            outputs = (1 << 1) | (1 << 2);
            break;
        default:
            outputs = 0;
            break;
    }

    return outputs;

}


uint8_t lookupf(float val, float* table, uint8_t size)
{
    uint8_t i = 0;
    for (i = 0; i < size; i++)
    {
        if ((table[i] >= val)) break;
    }
    return i;

}
