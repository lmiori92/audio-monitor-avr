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

/*#define STACK_MONITORING*/    /**< Enable stack monitoring */

#include "stdint.h"
#include "stdbool.h"

/** Output values */
typedef struct
{

    uint8_t relays;    /**< bits representing relay state */

} t_output;

/** Timer structure */
typedef struct
{
    bool     flag;
    uint32_t timestamp;
} t_timer32;

/** Timer structure */
typedef struct
{
    bool     flag;
    uint8_t  timestamp;
} t_timer8;

/** Operational variables */
typedef struct
{

    uint32_t   cycle_time;        /**< Time it takes the logic to execute */
    t_output   output;            /**< State of the outputs */
    uint8_t    reset_reason;      /**< Reset reason (see datasheet) */
    uint16_t   adc_min_ref;       /**< Minimum ADC attenuation (dB) */
    uint16_t   adc_max;           /**< Maximum ADC value (0dB) */
    t_timer32  flag_10ms;         /**< Set to true for one cycle every 10 ms */
    t_timer8   flag_50ms;         /**< Set to true for one cycle every 50 ms */
} t_operational;

/* Definitions */
#define FLAG_10MS_US        10000U      /** 10ms = 10000us */
#define FLAG_50MS_US        50000U      /** 50ms = 50000us */

#define FLAG_1000MS_50MS_UNITS      20U     /** 1000ms = 20 50ms-units */
#define FLAG_50MS_10MS_UNITS        5U

/* Globals */
extern uint8_t _end;
extern uint8_t __stack;

/* Functions */
uint8_t system_init(void);
void system_fatal(char *str);
void system_reset(void);

void StackPaint(void) __attribute__ ((naked)) __attribute__ ((section (".init1")));

#endif /* SRC_SYSTEM_H_ */
