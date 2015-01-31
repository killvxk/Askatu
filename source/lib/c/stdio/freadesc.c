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

#include <sys/common.h>
#include <sys/esccodes.h>
#include <stdio.h>

#include "iobuf.h"

int freadesc(FILE *f,int *n1,int *n2,int *n3) {
	size_t i;
	char ec,askatu[MAX_ESCC_LENGTH];
	const char *askPtr = (const char*)askatu;
	for(i = 0; i < MAX_ESCC_LENGTH - 1 && (ec = RETERR(bgetc(f))) != ']'; i++)
		askatu[i] = ec;
	if(i < MAX_ESCC_LENGTH - 1 && ec == ']')
		askatu[i] = ec;
	askatu[i + 1] = '\0';
	return escc_get(&askPtr,n1,n2,n3);
}
