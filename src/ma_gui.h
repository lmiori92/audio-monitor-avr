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
 * @file ma_gui.h
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Header for the Graphical User Interface
 */

/** This structure describes a menu entry */

#ifndef __MA_GUI__
#define __MA_GUI__

#include "stddef.h"
#include "stdint.h"
#include "stdbool.h"

#define DEBOUNCE_BUTTONS   50000   /**< keypad debounce in us */

#define REASON_HOOVER   0
#define REASON_SELECT   1

struct menu_entry_
{

    char*                label;
    struct menu_page_* (*cb)(uint8_t reason, uint8_t id, struct menu_page_* page);

};

typedef struct menu_entry_ t_menu_entry;

/** Structure that holds information about a menu page */
struct menu_page_
{

    struct menu_page_* page_previous;  /**< Pointer to the previous page */
    void (*pre)(void);          /**< Pointer to a function that is called when the menu page is first shown */
    void (*post)(void);         /**< Pointer to a function that is called when the menu page is quit (back to another menu / shutdown) */
    t_menu_entry* const entries;      /**< Menu entries */
    uint8_t elements;

};

typedef struct menu_page_ t_menu_page;

typedef struct
{

    uint8_t      index;
    t_menu_page* page;
    bool         refresh;
    void         (*refresh_menu)(void);

} t_menu;

enum e_buttons_
{

    BUTTON_DOWN,
    BUTTON_SELECT,
    BUTTON_UP,

    NUM_BUTTONS

};

typedef struct
{

    bool       input[NUM_BUTTONS];
    uint32_t   debounce[NUM_BUTTONS];
    bool       latches[NUM_BUTTONS];
    bool       buttons[NUM_BUTTONS];

} t_keypad;

typedef enum e_buttons_ t_button;

/* Menu */
void ma_gui_init(t_menu* menu, t_keypad* keypad, t_menu_page* start_page);
void ma_gui_periodic(t_menu* menu, t_keypad* keypad);
void keypad_periodic(t_keypad* keypad);

/* Menu utils */
void ma_gui_page_change(t_menu *menu, t_menu_page *page_next);
t_menu_page* ma_gui_menu_goto_previous(uint8_t reason, uint8_t id, t_menu_page* page);

/* Graphics functions */
void display_string_len(char* string, uint8_t len);
void display_string(char* string);
void display_string_center(char* string);
void display_clear(void);
void display_load_bars_vert();
void display_load_bars_horiz();
void display_load_vumeter_bars_in_ram();
void display_show_horizontal_bar(uint8_t level);
void display_show_vertical_bars(uint8_t bar, uint8_t level);

#endif

