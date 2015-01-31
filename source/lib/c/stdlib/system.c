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
#include <sys/proc.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

int system(const char *cmd) {
	int child;
	sExitState state;
	/* check whether we have a shell */
	if(cmd == NULL) {
		int fd = open("/bin/shell",O_RDONLY);
		if(fd >= 0) {
			close(fd);
			return EXIT_SUCCESS;
		}
		return EXIT_FAILURE;
	}

	child = fork();
	if(child == 0) {
		const char *args[] = {"/bin/shell","-e",NULL,NULL};
		args[2] = cmd;
		execv(args[0],args);

		/* if we're here there is something wrong */
		error("Exec of '%s' failed",args[0]);
	}
	else if(child < 0)
		error("Fork failed");

	/* wait and return exit-code */
	if((child = waitchild(&state,-1)) < 0)
		return child;
	return state.exitCode;
}
