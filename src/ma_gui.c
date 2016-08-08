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
 * @file ma_gui.c
 * @author Lorenzo Miori
 * @date Oct 2015
 * @brief Graphical User Interface routines
 */

#include "ma_gui.h"

#include "keypad.h"

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "time.h"

#include "lc75710_graphics.h"

#include "ma_strings.h"     /* String table */

static void ma_gui_menu_display_entry(t_menu* menu)
{

    display_clear();

    if (menu != NULL && menu->page->entries[menu->index].label < STRING_NUM_IDS)
    {
        display_string_center(g_string_table[menu->page->entries[menu->index].label]);
    }

}

t_menu_page* ma_gui_menu_goto_previous(uint8_t reason, uint8_t id, t_menu_page* page)
{
    if (reason == REASON_SELECT)
        return page->page_previous;
    return NULL;
}

void ma_gui_init(t_menu* menu, t_menu_page* start_page)
{

    /* First selected page: audio sources */
    menu->page            = start_page;
    menu->page->page_previous = NULL;
    menu->index           = 0;
  
    ma_gui_menu_display_entry(menu);

}

void ma_gui_page_change(t_menu *menu, t_menu_page *page_next)
{
    if (page_next != NULL)
    {

        menu->page = page_next;
        menu->index = 0;
        menu->refresh = true;

        /* call the pre function */
        if (menu->page->pre_post != NULL)
            menu->page->pre_post(REASON_PRE);

    }
}

/*

 */
bool ma_gui_periodic(t_menu* menu)
{

    t_menu_page* page_next = NULL;
    bool refreshed = false;

    if ((menu->index > 0) && (keypad_clicked(BUTTON_UP) == KEY_CLICK))
    {
        menu->index--;
        menu->refresh = true;
    }
    else if (((menu->index + 1) < menu->page->elements) && (keypad_clicked(BUTTON_DOWN) == KEY_CLICK))
    {
        menu->index++;
        menu->refresh = true;
    }
    else if ((keypad_clicked(BUTTON_SELECT) == KEY_CLICK))
    {
        if (menu->page->entries[menu->index].cb != NULL)
        {
            page_next = menu->page->entries[menu->index].cb(REASON_SELECT, menu->index, menu->page);

            ma_gui_page_change(menu, page_next);
        }

    }
    
    if (menu->refresh == true)
    {
        refreshed = true;
        menu->refresh = false;
        ma_gui_menu_display_entry(menu);
        if (menu->page->entries[menu->index].cb != NULL)
            menu->page->entries[menu->index].cb(REASON_HOOVER, menu->index, menu->page);
    }

    return refreshed;

}

