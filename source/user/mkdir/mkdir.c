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

#include <sys/cmdargs.h>
#include <sys/common.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(const char *name) {
	fprintf(stderr,"Usage: %s <path> ...\n",name);
	exit(EXIT_FAILURE);
}

int main(int argc,const char *argv[]) {
	const char **args;

	int res = ca_parse(argc,argv,0,"");
	if(res < 0) {
		printe("Invalid arguments: %s",ca_error(res));
		usage(argv[0]);
	}
	if(ca_hasHelp())
		usage(argv[0]);

	args = ca_getFree();
	while(*args) {
		if(mkdir(*args,DIR_DEF_MODE) < 0)
			printe("Unable to create directory '%s'",*args);
		args++;
	}
	return EXIT_SUCCESS;
}
