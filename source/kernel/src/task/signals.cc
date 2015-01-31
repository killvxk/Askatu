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
#include <task/signals.h>
#include <task/thread.h>
#include <assert.h>
#include <atomic.h>
#include <common.h>
#include <util.h>

Signals::handler_func Signals::setHandler(Thread *t,int signal,handler_func func) {
	assert(signal < SIG_COUNT);
	assert(t == Thread::getRunning());
	handler_func old = t->sigHandler[signal];
	t->sigHandler[signal] = func;
	return old;
}

bool Signals::checkAndStart(Thread *t,int *sig,handler_func *handler) {
	assert(t == Thread::getRunning());
	if(t->sigmask != 0) {
		for(int i = 0; i < SIG_COUNT; ++i) {
			if(t->sigmask & (1 << i)) {
				/* take care not to lose a signal */
				Atomic::fetch_and_and(&t->sigmask,~(1 << i));
				if(t->sigHandler[i] == SIG_DFL) {
					if(isFatal(i)) {
						Proc::terminate(1,i);
						A_UNREACHED;
					}
					continue;
				}
				*sig = i;
				*handler = t->sigHandler[i];
				t->sigHandler[i] = SIG_DFL;
				return true;
			}
		}
	}
	return false;
}

bool Signals::addSignalFor(Thread *t,int signal) {
	assert(signal < SIG_COUNT);
	if(isFatal(signal) || t->sigHandler[signal] != SIG_DFL) {
		Atomic::fetch_and_or(&t->sigmask,1 << signal);
		if(!t->isIgnoringSigs())
			t->unblock();
		return true;
	}
	return false;
}

const char *Signals::getName(int signal) {
	static const char *names[] = {
		"SIGKILL",
		"SIGTERM",
		"SIGFPE",
		"SIGILL",
		"SIGINT",
		"SIGSEGV",
		"SIGCHLD",
		"SIGALRM",
		"SIGUSR1",
		"SIGUSR2",
		"SIGCANCEL",
	};
	static_assert(ARRAY_SIZE(names) == SIG_COUNT,"Signal names out of sync");
	if(signal < SIG_COUNT)
		return names[signal];
	return "Unknown signal";
}

void Signals::print(const Thread *t,OStream &os) {
	os.writef("Signals:\n");
	for(int i = 0; i < SIG_COUNT; ++i) {
		if(t->sigHandler[i])
			os.writef("\t%s: %u\n",getName(i),(t->sigmask & (1 << i)) ? 1 : 0);
	}
}
