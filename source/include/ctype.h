/**
 * $Id$
 * Copyright (C) 2008 - 2014 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#pragma once

#include <sys/common.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define CT_NUMERIC	1
#define CT_LOWER	2
#define CT_UPPER	4
#define CT_SPACE	8
#define CT_HEX		16
#define CT_PUNCT	32
#define CT_CTRL		64
#define CT_BLANK	128

/**
 * @param c the character
 * @return non-zero if its argument is a numeric digit or a letter of the alphabet.
 * 	Otherwise, zero is returned.
 */
int isalnum(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a letter of the alphabet. Otherwise, zero is returned.
 */
int isalpha(int c);

/**
 * @param c the character
 * @return non-zero if its argument is ' ' or '\t'. Otherwise, zero is returned.
 */
int isblank(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a control-character ( < 0x20 )
 */
int iscntrl(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a digit between 0 and 9. Otherwise, zero is returned.
 */
int isdigit(int c);

/**
 * @param c the character
 * @return non-zero if its argument has a graphical representation. Otherwise, zero is returned.
 */
int isgraph(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a lowercase letter. Otherwise, zero is returned.
 */
int islower(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a printable character
 */
int isprint(int c);

/**
 * @param c the character
 * @return non-zero if its argument is a punctuation character
 */
int ispunct(int c);

/**
 * @param c the character
 * @return non-zero if its argument is some sort of space (i.e. single space, tab,
 * 	vertical tab, form feed, carriage return, or newline). Otherwise, zero is returned.
 */
int isspace(int c);

/**
 * @param c the character
 * @return non-zero if its argument is an uppercase letter. Otherwise, zero is returned.
 */
int isupper(int c);

/**
 *
 * @param c the character
 * @return non-zero if its argument is a hexadecimal digit
 */
int isxdigit(int c);

/**
 * @param ch the char
 * @return the lowercase version of the character ch.
 */
int tolower(int ch);

/**
 * @param ch the char
 * @return the uppercase version of the character ch.
 */
int toupper(int ch);

#if defined(__cplusplus)
}
#endif
