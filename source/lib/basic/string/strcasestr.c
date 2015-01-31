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

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>

char *strcasestr(const char *str1,const char *str2) {
	char *res = NULL;
	char *sub;

	vassert(str1 != NULL,"str1 == NULL");
	vassert(str2 != NULL,"str2 == NULL");

	/* handle special case to prevent looping the string */
	if(!*str2)
		return (char*)str1;
	while(*str1) {
		/* matching char? */
		if(tolower(*str1++) == tolower(*str2)) {
			res = (char*)--str1;
			sub = (char*)str2;
			/* continue until the strings don't match anymore */
			while(*sub && tolower(*str1) == tolower(*sub)) {
				str1++;
				sub++;
			}
			/* complete substring matched? */
			if(!*sub)
				return res;
		}
	}
	return NULL;
}
