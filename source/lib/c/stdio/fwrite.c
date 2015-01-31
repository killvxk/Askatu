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
#include <sys/io.h>
#include <stdio.h>

#include "iobuf.h"

size_t fwrite(const void *ptr,size_t size,size_t count,FILE *file) {
	/* first flush the output */
	int res = fflush(file);
	if(file->out.fd < 0 || res < 0)
		return 0;
	/* TODO like in fread, we could write to buffer if its less than the buffer-size and so on */
	res = write(file->out.fd,ptr,count * size);
	if(res < 0) {
		file->error = res;
		return 0;
	}
	return res / size;
}
