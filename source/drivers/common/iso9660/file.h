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

#include "iso9660.h"

class ISO9660File {
	ISO9660File() = delete;

public:
	/**
	 * Reads <count> bytes at <offset> into <buffer> from the given file-id
	 *
	 * @param h the iso9660-handle
	 * @param id the file-id
	 * @param buffer the buffer; if NULL the data will be fetched from disk (if not in cache) but
	 * 	not copied anywhere
	 * @param offset the offset
	 * @param count the number of bytes to read
	 * @return the number of read bytes
	 */
	static ssize_t read(ISO9660FileSystem *fs,ino_t id,void *buffer,off_t offset,size_t count);

private:
	static void buildDirEntries(ISO9660FileSystem *h,block_t lba,uint8_t *dst,const uint8_t *src,
		off_t offset,size_t count);
};
