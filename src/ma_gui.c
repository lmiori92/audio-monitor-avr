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

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "time.h"

#include "lc75710_graphics.h"

#include "ma_strings.h"     /* String table */

void ma_gui_menu_display_entry(t_menu* menu)
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

void ma_gui_init(t_menu* menu, t_keypad* keypad, t_menu_page* start_page)
{

    uint8_t i = 0;

    /* Init keypad */
    for (i = 0; i < NUM_BUTTONS; i++)
    {
        keypad->input[i] = false;
        keypad->buttons[i]  = false;
        keypad->latches[i]  = false;
        keypad->debounce[i] = 0;
    }

    /* First selected page: audio sources */
    menu->page            = start_page;
    menu->page->page_previous = NULL;
    menu->index           = 0;
  
    ma_gui_menu_display_entry(menu);

}

/* Read the keypad, apply debounce to inputs and detect the rising edge */
void keypad_periodic(t_keypad* keypad)
{

  uint8_t i = 0;
  bool t = false;

  for (i = 0; i < NUM_BUTTONS; i++)
  {
      t = keypad->input[i];
      
      if (t == true)
      {
        t = false;

        /* debounce the raw input */
        if (keypad->debounce[i] == 0)
          keypad->debounce[i] = g_timestamp;
        else
          if ((g_timestamp - keypad->debounce[i]) > DEBOUNCE_BUTTONS)
            t = true;
      }
      else
      {
          keypad->debounce[i] = 0;
          t = false;
      }
      
      if (t == true && keypad->latches[i] == false)
      {
          /* Falling edge */
          keypad->buttons[i] = true;
      }
      else
      {
          keypad->buttons[i] = false;
      }

      keypad->latches[i] = t;
  }

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
void ma_gui_periodic(t_menu* menu, t_keypad* keypad)
{

    t_menu_page* page_next = NULL;

    if ((menu->index > 0) && keypad->buttons[BUTTON_UP] == true)
    {
        menu->index--;
        menu->refresh = true;
    }
    else if (((menu->index + 1) < menu->page->elements) && keypad->buttons[BUTTON_DOWN] == true)
    {
        menu->index++;
        menu->refresh = true;
    }
    else if (keypad->buttons[BUTTON_SELECT] == true)
    {
        if (menu->page->entries[menu->index].cb != NULL)
        {
            page_next = menu->page->entries[menu->index].cb(REASON_SELECT, menu->index, menu->page);

            ma_gui_page_change(menu, page_next);
        }

    }
    
    if (menu->refresh == true)
    {
        menu->refresh = false;
        ma_gui_menu_display_entry(menu);
        if (menu->page->entries[menu->index].cb != NULL)
            menu->page->entries[menu->index].cb(REASON_HOOVER, menu->index, menu->page);
    }

}

