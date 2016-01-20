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
 * @file manage_audio.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief The main routines calling the logic functions
 */

#include "lc75710.h"
#include "lc75710_graphics.h"
#include "time.h"
#include "ma_gui.h"
#include "ma_util.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "ffft.h"

/* AVR libs */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

/* UART */
#include "uart.h"
#include "ma_audio.h"
#include "system.h"
#include "string_table.h"

/* Globals */
t_operational operational;          /**< Global operational state */

static t_menu   menu;               /**< Menu state */
static t_keypad keypad;             /**< Keypad state */
static t_persistent persistent;     /**< Persistent app state */


/* PIN definitions */
#define RLY_DDR     DDRD
#define RLY_PORT    PORTD
#define RLY_1       PD5
#define RLY_2       PD6
#define RLY_3       PD7

#define KEY_PIN     PINB
#define KEY_1       PB0
#define KEY_2       PB1
#define KEY_3       PB2

/* Local function declaration */

t_menu_page* ma_gui_source_select(uint8_t reason, uint8_t id, t_menu_page* page);
t_menu_page* ma_gui_menu_goto_sett_display(uint8_t reason, uint8_t id, t_menu_page* page);
t_menu_page* ma_gui_menu_goto_sett_brightness(uint8_t reason, uint8_t id, t_menu_page* page);
t_menu_page* ma_gui_menu_set_brightness(uint8_t reason, uint8_t id, t_menu_page* page);
t_menu_page* ma_gui_menu_goto_tools(uint8_t reason, uint8_t id, t_menu_page* page);
t_menu_page* ma_gui_menu_tools_selection(uint8_t reason, uint8_t id, t_menu_page* page);

void ma_gui_settings_brightness_pre();
void ma_gui_source_select_pre();
void set_display_brightness(uint8_t level);

/* Source menu entries */
t_menu_entry  MENU_SOURCE[] = {
                            { .label = "AUX",            .cb = &ma_gui_source_select },
                            { .label = "CD",             .cb = &ma_gui_source_select },
                            { .label = "RADIO",          .cb = &ma_gui_source_select },
                            { .label = "TAPE",           .cb = &ma_gui_source_select },
};

/* Source menu page */
t_menu_page PAGE_SOURCE = {
    .page_previous = NULL,
    .pre     = &ma_gui_source_select_pre,
    .post    = NULL,
    .entries = MENU_SOURCE,
    .elements = sizeof(MENU_SOURCE) / sizeof(t_menu_entry)
};

t_menu_entry MENU_SETTINGS[] = {
                            { .label = "Sources",        .cb = NULL },
                            { .label = "Display",        .cb = &ma_gui_menu_goto_sett_display },
                            { .label = "FFT",            .cb = NULL    },
                            { .label = "VU-Meter",       .cb = NULL     },
                            { .label = "Tools",          .cb = &ma_gui_menu_goto_tools     },
                            { .label = "BACK",           .cb = &ma_gui_menu_goto_previous },
};

t_menu_page PAGE_SETTINGS = {
    .page_previous = &PAGE_SOURCE,
    .pre     = NULL,
    .post    = NULL,
    .entries = MENU_SETTINGS,
    .elements = sizeof(MENU_SETTINGS) / sizeof(t_menu_entry)
};

t_menu_entry  MENU_SETTINGS_DISPLAY[] = {
                            { .label = "Brightness",    .cb = &ma_gui_menu_goto_sett_brightness  },
                            { .label = "Menu Style",    .cb = NULL  },
                            { .label = "BACK",          .cb = &ma_gui_menu_goto_previous },
};

t_menu_page PAGE_SETTINGS_DISPLAY = {
    .page_previous = &PAGE_SETTINGS,
    .pre     = NULL,
    .post    = NULL,
    .entries = MENU_SETTINGS_DISPLAY,
    .elements = sizeof(MENU_SETTINGS_DISPLAY) / sizeof(t_menu_entry)

};
                           
t_menu_entry  MENU_SETTINGS_BRIGHTNESS[] =
{
		{ .label = "1: TeSt!*",        .cb = &ma_gui_menu_set_brightness  },
        { .label = "2: TeSt!*",        .cb = &ma_gui_menu_set_brightness  },
        { .label = "3: TeSt!*",        .cb = &ma_gui_menu_set_brightness  },
        { .label = "4: TeSt!*",        .cb = &ma_gui_menu_set_brightness  },
        { .label = "5: TeSt!*",        .cb = &ma_gui_menu_set_brightness  },
};

t_menu_page PAGE_SETTINGS_BRIGHTNESS = {
        .page_previous = &PAGE_SETTINGS_DISPLAY,
    .pre     = ma_gui_settings_brightness_pre,
    .post    = NULL,
    .entries = MENU_SETTINGS_BRIGHTNESS,
    .elements = sizeof(MENU_SETTINGS_BRIGHTNESS) / sizeof(t_menu_entry)
};

t_menu_entry  MENU_SETTINGS_TOOLS[] = {
        { .label = "Debug",         .cb = &ma_gui_menu_tools_selection  },
                            { .label = "Reboot",        .cb = &ma_gui_menu_tools_selection  },
                            { .label = "BACK", .cb = &ma_gui_menu_goto_previous },
};

t_menu_page PAGE_SETTINGS_TOOLS = {
    .page_previous = &PAGE_SETTINGS,
    .pre     = NULL,
    .post    = NULL,
    .entries = MENU_SETTINGS_TOOLS,
    .elements = sizeof(MENU_SETTINGS_TOOLS) / sizeof(t_menu_entry)
};

t_menu_entry MENU_DEBUG[] = {
                {.label = NULL, .cb = NULL},
                {.label = NULL, .cb = NULL},
                {.label = NULL, .cb = NULL},
                {.label = NULL, .cb = NULL},
                {.label = NULL, .cb = NULL},
                {.label = NULL, .cb = NULL},
                { .label = "BACK", .cb = &ma_gui_menu_goto_previous },
};

t_menu_page PAGE_DEBUG = {
        .page_previous = &PAGE_SETTINGS_TOOLS,
        .post    = NULL,
        .pre     = NULL,
        .entries = MENU_DEBUG,
        .elements = sizeof(MENU_DEBUG) / sizeof(t_menu_entry)
};

void ma_gui_settings_brightness_pre()
{
    if (persistent.brightness < PAGE_SETTINGS_BRIGHTNESS.elements)
        menu.index = persistent.brightness;
}

void ma_gui_source_select_pre()
{
    if (persistent.audio_source < PAGE_SOURCE.elements)
    {
        menu.index = persistent.audio_source;
    }
}

t_menu_page* ma_gui_source_select(uint8_t reason, uint8_t id, t_menu_page* page)
{

    if (reason == REASON_HOOVER)
    {
        persistent.audio_source = id;
        operational.output.relays = source_select(id);
        write_to_persistent(&persistent);
    }
    else if (reason == REASON_SELECT)
    {
        return &PAGE_SETTINGS;
    }

    return NULL;

}

/* MENU: callbacks */

t_menu_page* ma_gui_menu_goto_sett_display(uint8_t reason, uint8_t id, t_menu_page* page)
{
    if (reason == REASON_SELECT)
        return &PAGE_SETTINGS_DISPLAY;
    else
        return NULL;
}

t_menu_page* ma_gui_menu_goto_sett_brightness(uint8_t reason, uint8_t id, t_menu_page* page)
{
    if (reason == REASON_SELECT)
        return &PAGE_SETTINGS_BRIGHTNESS;
    else
        return NULL;
}

t_menu_page* ma_gui_menu_set_brightness(uint8_t reason, uint8_t id, t_menu_page* page)
{
    if (reason == REASON_HOOVER)
        set_display_brightness(id);
    else
        return ma_gui_menu_goto_previous(reason, id, page);
    return NULL;
}

t_menu_page* ma_gui_menu_goto_tools(uint8_t reason, uint8_t id, t_menu_page* page)
{
    if (reason == REASON_SELECT)
        return &PAGE_SETTINGS_TOOLS;
    else
        return NULL;
}

t_menu_page* ma_gui_menu_tools_selection(uint8_t reason, uint8_t id, t_menu_page* page)
{

    void (*start)(void) = 0;

    if (reason == REASON_SELECT)
    {
        switch(id)
        {
            case 0:
                return &PAGE_DEBUG;
            case 1:
                start();
                break;
            default:
                break;
        }
    }

    return NULL;

}

static uint32_t sum = 0;
static uint32_t filtered_value = 0;

/* This is a complete work in progress :) */

float table[] = {
        -35.56f,
        -15.56f,
        -9.54f,
        -6.02f,
        -3.52f,
        -1.58f,
        0.0f,
//        -18.62f,
//        -17.28f
};


char disp_str[12];
float x_dB = 0;
float minref = 36.12f;
float adc_max = 64.0f;
uint8_t t;
void ma_gui_refresh()
{
    uint8_t i,j;
    uint16_t *spektrum = ma_audio_spectrum();
    if (menu.page == &PAGE_DEBUG)
    {
menu.index = 4;
        switch (menu.index)
        {

            case 0:
                snprintf_P(disp_str, 11, PSTR("%lu ms"), g_timestamp / 1000);
                display_clear();
                display_string(disp_str);

                break;

            case 1:

                snprintf_P(disp_str, 11, PSTR("CT: %lu ms"), operational.cycle_time / 1000);
                display_clear();
                display_string(disp_str);

                break;

            case 2:

                snprintf_P(disp_str, 11, PSTR("CM: %lu ms"), operational.cycle_time_max / 1000);
                display_clear();
                display_string(disp_str);

                break;

            case 3:
                snprintf_P(disp_str, 11, PSTR("RELAY: %d%d%d"),
                                  ((operational.output.relays >> 0) & 1),
                                  ((operational.output.relays >> 1) & 1),
                                  ((operational.output.relays >> 2) & 1));
                display_clear();
                display_string(disp_str);

                break;

            case 4:

                for (i = 0; i < (FFT_N/2/3); i++)
                {

                    //float v = log10f(spektrum[i] / 60.0f) * 20.0f;
                    //display_show_vertical_bars(i, lookupf(v, table, sizeof(table) / sizeof(float)));
                    uint16_t v = 0;
                    v = (spektrum[i*3] + spektrum[i*3+1] + spektrum[i*3+2]);

                    if (v > 0)
                    {
                        x_dB = 20.0f * log10(v / adc_max);
                        t = minref - fabs(x_dB);

                    }
                    else
                    {
                        t = 0;
                    }

                    display_show_vertical_bars(i, (t / minref) * 6.0f);
//                    display_show_vertical_bars(i, lookupf(x_dB, table, sizeof(table) / sizeof(float)));//spektrum[i] / 10);
                }

                /*
                float v = log10f(spektrum[2] / 1024.0f) * 20.0f;
                display_clear();
                uint16_t vp = v;
                snprintf(disp_str, 11, "%d ; %d", vp, lookupf(v, table, sizeof(table) / sizeof(float)));
                display_string(disp_str);
                */

//                float v = abs(log10f(spektrum[2] / 1024.0f) * 20.0f);
//                uint16_t c = ceil(v);
//                display_clear();
//                uint16_t v = (uint16_t)round(abs(20.0f * log10(spektrum[2] / 1024.0f)));
//                snprintf_P(disp_str, 11, PSTR("%u-%u"), spektrum[2], c);
//                display_string(disp_str);

                /*
                display_show_vertical_bars(0, ((spektrum[1] >> 2) + (spektrum[2] >> 2))  );
            	display_show_vertical_bars(1, ((spektrum[3] >> 2) + (spektrum[4] >> 2))  );
            	display_show_vertical_bars(2, ((spektrum[5] >> 2) + (spektrum[6] >> 2))  );
            	display_show_vertical_bars(3, ((spektrum[7] >> 2) + (spektrum[8] >> 2))  );
            	display_show_vertical_bars(4, ((spektrum[9] >> 2) + (spektrum[10] >> 2)) );
            	display_show_vertical_bars(5, ((spektrum[11] >> 2) + (spektrum[12] >> 2)));
            	display_show_vertical_bars(6, ((spektrum[13] >> 2) + (spektrum[14] >> 2)));
            	display_show_vertical_bars(7, ((spektrum[15] >> 2) + (spektrum[16] >> 2)));
            	display_show_vertical_bars(8, ((spektrum[17] >> 2) + (spektrum[18] >> 2)));
            	display_show_vertical_bars(9, ((spektrum[19] >> 2) + (spektrum[20] >> 2)));
            	*/
            	break;

            case 5:

                #define filter_strength 4


                sum = sum - filtered_value + ma_audio_last_capture();
                filtered_value = (sum + (1<<(filter_strength - 1))) >> (filter_strength);

                display_clear();
                snprintf_P(disp_str, 11, PSTR("ADC: %d"), filtered_value);
//                snprintf_P(disp_str, 11, PSTR("%lu"), g_timestamp / operational.adc_samples);
                display_string(disp_str);

                break;

            case 6:
                display_clear();
//                snprintf(disp_str, 11, "ADC: %d", ma_audio_last_capture());
                snprintf_P(disp_str, 11, PSTR("RST: %d"), operational.reset_reason);
                display_string(disp_str);

                break;

            case 7:

                display_clear();
                snprintf_P(disp_str, 11, PSTR("%d%d%d"), last_capture, adc_maxS, adc_minS);
                display_string(disp_str);

                break;

            default:
                break;
        }
    }

}

/**
* set_display_brightness
*
* @brief Set application display brightness
*/
void set_display_brightness(uint8_t level)
{
    uint8_t brightness_levels[5] = { 48, 96, 144, 192, 240 };
    
    if (level < 5)
    {
        lc75710_intensity(brightness_levels[level]);
        persistent.brightness = level;
        write_to_persistent(&persistent);
    }
}

/**
 * persistent_defaults
 *
 * @brief Set all the persistent data to the default state.
 *        Useful when CRC mismatches
 * @param persistent the persistent variables to write to
 *
 */
void persistent_defaults(t_persistent* persistent)
{
    persistent->brightness = 0;
    persistent->audio_source = 0;
}

void io_init()
{

    /* I/O init */

    /* Audio Switch Relays init */
    RLY_DDR |= 1 << RLY_1;  /* OUTPUT */
    RLY_DDR |= 1 << RLY_2;  /* OUTPUT */
    RLY_DDR |= 1 << RLY_3;  /* OUTPUT */

    /* Keypad */
    DDRB &= ~(1 << KEY_1);  /* INPUT */
    DDRB &= ~(1 << KEY_2);  /* INPUT */
    DDRB &= ~(1 << KEY_3);  /* INPUT */
    PORTB |=  (1 << KEY_1);  /* PULL-UP */
    PORTB |=  (1 << KEY_2);  /* PULL-UP */
    PORTB |=  (1 << KEY_3);  /* PULL-UP */

}

void setup()
{

    /* Initialize the I/O */
    io_init();

    /* Initialze the display */
    lc75710_init();

    /* Timer: start ticking */
    timer_init();

    /* ADC */
    ma_audio_init();

    /* Wait Aref stabilization (0.47uF capacitance) */
    _delay_ms(500);

    /* Initialize the serial port */
    uart_init();
    stdout = &uart_output;
    stdin  = &uart_input;

    /* Load persistent data */
    read_from_persistent(&persistent);

    /* Initialize the GUI */
    ma_gui_init(&menu, &keypad, &PAGE_SOURCE);

    /* Apply persistent data */
    set_display_brightness(persistent.brightness);

}

void input(t_keypad *keypad)
{

    /* Keypad */
    keypad->input[BUTTON_SELECT]    = !((PINB >> KEY_1) & 0x1);
    keypad->input[BUTTON_UP]        = !((PINB >> KEY_2) & 0x1);
    keypad->input[BUTTON_DOWN]      = !((PINB >> KEY_3) & 0x1);

}

void output()
{

    /* Relays */
    (operational.output.relays      & 0x1) ? (RLY_PORT |= 1 << RLY_1) : (RLY_PORT &= ~(1 << RLY_1));
    (operational.output.relays >> 1 & 0x1) ? (RLY_PORT |= 1 << RLY_2) : (RLY_PORT &= ~(1 << RLY_2));
    (operational.output.relays >> 2 & 0x1) ? (RLY_PORT |= 1 << RLY_3) : (RLY_PORT &= ~(1 << RLY_3));

}

int main(void)
{

    uint32_t start = 0;

    /* Disable interrupts for the whole init period */
    cli();

    /* System stuff */
    system_init();

    /* Wait for power and LC75710 stabilization */
    _delay_ms(500);

    /* Setup the peripherals */
    setup();

    /* Start the application: re-enable interrupts */
    sei();

    /* Set periodic menu handler */
    menu.refresh_menu = ma_gui_refresh;

    /* Load CGRAM data */
    display_load_bars_vert();
    uint8_t i, j = 0;
//    display_load_bars_horiz();
    //while (1) {
        /* vu-meter left to right test*/
        /*
        display_load_vumeter_harrows();
        display_show_vumeter_harrows(i, j, true);
        */
//        display_show_horizontal_bar(i);
//        i++;
//        i  %= 100;
//        j++;
//        j  %= 10;
//        _delay_us(25);
    //}

    if (!((PINB >> KEY_1) & 0x1))
    {
        /* Directly go to the debug menu */
        ma_gui_page_change(&menu, &PAGE_DEBUG);
    }
    else
    {
        /* Start with the SOURCE menu */
        ma_gui_page_change(&menu, &PAGE_SOURCE);
    }
    ma_gui_page_change(&menu, &PAGE_DEBUG);
    /* Start the main loop (and never return) */
    while (1)
    {

        /* Cycle start */
        start = g_timestamp;

        /* Read inputs */
        input(&keypad);

        /* Keypad logic */
        keypad_periodic(&keypad);

        /* Process audio (FFT / VU-METER) */
        ma_audio_process();

        /* Run the periodic GUI logic */
        ma_gui_periodic(&menu, &keypad);

        /* Set outputs */
        output();

        /* Cycle end */
        operational.cycle_time = g_timestamp - start;

        /* Save peak cycle time */
        if (operational.cycle_time > operational.cycle_time_max)
        {
            operational.cycle_time_max = operational.cycle_time;
        }

        /* Check stack sanity */
        if (StackCount() == 0U)
        {
            display_clear();
            display_string(PSTR("StackOver!"));
            for(;;);
        }

    }

}

void shutdown()
{
    /* Save to EEPROM */
}
