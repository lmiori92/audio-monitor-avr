
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

    Lorenzo Miori (C) 2015-2016 [ 3M4|L: memoryS60<at>gmail.com ]

    Version History
        * 1.0 initial

*/

/**
 * @file ma_strings.h
 * @author Lorenzo Miori
 * @date Jan 2016
 * @brief Header file for the string table
 */

#ifndef SRC_STRING_TABLE_H_
#define SRC_STRING_TABLE_H_

extern const char* g_string_table[];  /**< Global string table */

#endif  /* SRC_STRING_TABLE_H_ */


enum _string_table_ids
{
    STRING_AUX,  /**< AUX */
    STRING_CD,  /**< CD */
    STRING_RADIO,  /**< RADIO */
    STRING_TAPE,  /**< TAPE */
    STRING_SOURCES,  /**< SOURCES */
    STRING_DISPLAY,  /**< DISPLAY */
    STRING_FFT,  /**< FFT */
    STRING_VU_HORIZ,  /**< VU-HORIZ */
    STRING_VU_VERT,  /**< VU-VERT */
    STRING_TOOLS,  /**< TOOLS */
    STRING_BACK,  /**< BACK */
    STRING_BRIGHTNESS,  /**< BRIGHTNESS */
    STRING_METER,  /**< METER */
    STRING_REBOOT,  /**< REBOOT */
    STRING_DEBUG,  /**< DEBUG */
    STRING_TEST,  /**< TEST!* */
    STRING_SW_VERSION,

    STRING_NUM_IDS
};

