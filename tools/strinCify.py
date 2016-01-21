
import sys

ENUM_TEMPLATE = \
'''
enum _string_table_ids
{
PLACEHOLDER_STRING_LIST
    STRING_NUM_IDS
};
'''

ARRAY_TEMPLATE = \
'''
/* STRING SIZE XXXBYTESXXX BYTES */
const char* g_string_table[] = 
{
PLACEHOLDER_STRING_LIST
};
'''

HEADER_TEMPLATE = \
'''
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
 * @file PLACEHOLDER_FILENAME.h
 * @author Lorenzo Miori
 * @date Jan 2016
 * @brief Header file for the string table
 */

#ifndef SRC_STRING_TABLE_H_
#define SRC_STRING_TABLE_H_

extern const char* g_string_table[];  /**< Global string table */

#endif  /* SRC_STRING_TABLE_H_ */

PLACEHOLDER_ENUM
'''

CLASS_TEMPLATE = \
'''
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
 * @file PLACEHOLDER_FILENAME.c
 * @author Lorenzo Miori
 * @date Jan 2016
 * @brief Source file for the string table
 */

#include "PLACEHOLDER_FILENAME.h"

PLACEHOLDER_ARRAY
'''

INVALID_C_CHARS = ["!","*"]

def Cify(s):
    
    s = s.replace(" ", "_")
    s = s.replace("-", "_")
    for char in INVALID_C_CHARS:
        s = s.replace(char, "");

    return s

def convert(filename):
    
    enumList = ""
    arrayList = ""
    totLen = 0
    
    fs = open(filename, "rb")
    
    strings = [ x.strip() for x in fs.readlines()]

    for string in strings:
        totLen += len(string) + 1 # null terminator (C)
        enumList += "    "
        arrayList += "    "
        s = Cify(string)
        enumList += ("STRING_%s,  /**< %s */" % (s, string)).upper()
        enumList += "\n"
        arrayList += ("\"%s\"," % (string))
        arrayList += "\n"

    resultEnum = ENUM_TEMPLATE.replace("PLACEHOLDER_STRING_LIST", enumList)
    resultArray = ARRAY_TEMPLATE.replace("PLACEHOLDER_STRING_LIST", arrayList).replace("XXXBYTESXXX", "%i" % totLen)

    print "Strings use %i bytes" % totLen

    fs.close()

    return resultEnum, resultArray

def create_header(filename, enum):

    fo = open("%s.h" % filename, "wb")
    
    fo.write(HEADER_TEMPLATE.replace("PLACEHOLDER_FILENAME", filename).replace("PLACEHOLDER_ENUM", enum))
    
    fo.close()

def create_source(filename, array):
    
    fo = open("%s.c" % filename, "wb")

    fo.write(CLASS_TEMPLATE.replace("PLACEHOLDER_FILENAME", filename).replace("PLACEHOLDER_ARRAY", array))
    
    fo.close()

if len(sys.argv) < 2:
    print "Please set the input file containing the string list."
    sys.exit(1)
else:
    enum, array = convert(sys.argv[1])
    if (len(sys.argv) > 2):
        fn = sys.argv[2]
    else:
        fn = "string_table"
    print enum
    print array
    create_header(fn, enum)
    create_source(fn, array)
