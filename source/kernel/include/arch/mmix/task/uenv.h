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

#include <common.h>

class UEnv : public UEnvBase {
	friend class UEnvBase;

	UEnv() = delete;

	static void startSignalHandler(Thread *t,int sig,Signals::handler_func handler);
	static void addArgs(Thread *t,const ELF::StartupInfo *info,ulong *rsp,ulong *ssp,
	                    uintptr_t entry,uintptr_t tentry,bool thread);
};

inline void UEnvBase::handleSignal(Thread *t,A_UNUSED IntrptStackFrame *stack) {
	int sig;
	Signals::handler_func handler;
	if(Signals::checkAndStart(t,&sig,&handler))
		UEnv::startSignalHandler(t,sig,handler);
}
