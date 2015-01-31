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

#include <dirent.h>
#include <string.h>

char *basename(char *path) {
	static char dot[] = ".";
	if(!path || !path[0])
		return dot;
	if((path[0] == '.' || path[0] == '/') && path[1] == '\0')
		return path;
	if(path[0] == '.' && path[1] == '.' && path[2] == '\0')
		return path;

	size_t len = strlen(path);
	while(len > 0 && path[len - 1] == '/')
		path[--len] = '\0';
	while(len > 0 && path[len - 1] != '/')
		len--;
	return path + len;
}
