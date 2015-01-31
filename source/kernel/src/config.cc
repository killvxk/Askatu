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

#include <task/proc.h>
#include <task/smp.h>
#include <task/thread.h>
#include <task/timer.h>
#include <common.h>
#include <config.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

bool Config::logToVGA = false;
bool Config::lineByLine = false;
bool Config::doLog = true;
bool Config::smp = true;
bool Config::forcePIT = false;
bool Config::forcePIC = false;
char Config::rootDev[MAX_BPVAL_LEN + 1] = "";
char Config::swapDev[MAX_BPVAL_LEN + 1] = "";

void Config::parseBootParams(int argc,const char *const *argv) {
	char name[MAX_BPNAME_LEN + 1];
	char value[MAX_BPVAL_LEN + 1];
	for(int i = 1; i < argc; i++) {
		size_t len = strlen(argv[i]);
		if(len > MAX_BPNAME_LEN + MAX_BPVAL_LEN + 1)
			continue;
		size_t eq = strchri(argv[i],'=');
		if(eq > MAX_BPNAME_LEN || (len - eq) > MAX_BPVAL_LEN)
			continue;
		strncpy(name,argv[i],eq);
		name[eq] = '\0';
		if(eq != len)
			strnzcpy(value,argv[i] + eq + 1,sizeof(value));
		else
			value[0] = '\0';
		set(name,value);
	}

	// TODO no SMP on x86_64 yet
#if defined(__x86_64__)
	smp = false;
#endif
}

const char *Config::getStr(int id) {
	const char *res = NULL;
	switch(id) {
		case ROOT_DEVICE:
			res = rootDev;
			break;
		case SWAP_DEVICE:
			res = *swapDev ? swapDev : NULL;
			break;
	}
	return res;
}

long Config::get(int id) {
	long res;
	switch(id) {
		case TIMER_FREQ:
			res = Timer::FREQUENCY_DIV;
			break;
		case MAX_PROCS:
			res = MAX_PROC_COUNT;
			break;
		case MAX_FDS:
			res = MAX_FD_COUNT;
			break;
		case LOG:
			res = doLog;
			break;
		case LOG_TO_VGA:
			res = logToVGA;
			break;
		case LINE_BY_LINE:
			res = lineByLine;
			break;
		case CPU_COUNT:
			res = SMP::getCPUCount();
			break;
		case SMP:
			res = smp;
			break;
		case TICKS_PER_SEC:
			res = CPU::getSpeed();
			break;
		case FORCE_PIT:
			res = forcePIT;
			break;
		case FORCE_PIC:
			res = forcePIC;
			break;
		default:
			res = -EINVAL;
			break;
	}
	return res;
}

void Config::set(const char *name,const char *value) {
	if(strcmp(name,"root") == 0)
		strcpy(rootDev,value);
	else if(strcmp(name,"swapdev") == 0)
		strcpy(swapDev,value);
	else if(strcmp(name,"nolog") == 0)
		doLog = false;
	else if(strcmp(name,"linebyline") == 0)
		lineByLine = true;
	else if(strcmp(name,"logtovga") == 0)
		logToVGA = true;
	else if(strcmp(name,"nosmp") == 0)
		smp = false;
	else if(strcmp(name,"forcepit") == 0)
		forcePIT = true;
	else if(strcmp(name,"forcepic") == 0)
		forcePIC = true;
}
