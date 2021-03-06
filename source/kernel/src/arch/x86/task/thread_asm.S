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

#include <sys/arch.h>

.global thread_idle

// void thread_idle();
thread_idle:
	// when starting to idle for the first time, ensure that the segment-registers are set correctly
	// for userland. this is important for APs, which start here (instead of going to userland like
	// the BSP) and wake up later by an interrupt and might leave into userland from there.
	mov		$((SEG_UDATA << 3) | 3),%eax
	mov		%eax,%ds
	mov		%eax,%es
	mov		%eax,%fs
	mov		%eax,%gs

	sti
1:
	hlt
	jmp			1b
