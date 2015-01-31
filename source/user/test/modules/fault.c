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
#include <sys/debug.h>
#include <sys/io.h>
#include <sys/proc.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../modules.h"

static void sig_segf(A_UNUSED int sig) {
	printf("GOT SIGSEGV!\n");
	exit(EXIT_FAILURE);
}

int mod_fault(A_UNUSED int argc,A_UNUSED char *argv[]) {
	uint *ptr;
	int fd;
	if(signal(SIGSEGV,sig_segf) == SIG_ERR)
		error("Unable to set signal-handler");
	printf("I am evil ^^\n");
	fd = open((char*)0x12345678,O_RDONLY);
	ptr = (uint*)0xFFFFFFFF;
	*ptr = 1;
	printf("Never printed\n");
	close(fd);
	return EXIT_SUCCESS;
}
