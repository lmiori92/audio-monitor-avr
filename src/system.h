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
 * @file system.h
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief System level utilities: ISR debugging / MCU power state
 */

#ifndef SRC_SYSTEM_H_
#define SRC_SYSTEM_H_

typedef struct
{

    uint8_t relays;    /**< bits representing relay state */

} t_output;

typedef struct
{

    uint32_t cycle_time;
    uint32_t cycle_time_max;
    uint32_t adc_samples;
    uint32_t adc_isr_time;
    t_output output;
    uint8_t reset_reason;

} t_operational;

/* Constants */
extern t_operational operational;

/* Functions */
void system_init(void);

#endif /* SRC_SYSTEM_H_ */
