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
#include <sys/log.h>
#include <sys/mman.h>
#include <assert.h>

#define OUTPUT_START_ADDR		0x3F000000

static uint32_t *outRegs = NULL;

void logc(char c) {
	if(outRegs == NULL) {
		uintptr_t phys = OUTPUT_START_ADDR;
		outRegs = mmapphys(&phys,8,0,MAP_PHYS_MAP);
		assert(outRegs != NULL);
	}
	*outRegs = c;
}
