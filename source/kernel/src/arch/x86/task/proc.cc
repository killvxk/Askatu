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

#include <arch/x86/task/ioports.h>
#include <task/proc.h>
#include <common.h>

int ProcBase::cloneArch(Proc *dst,A_UNUSED const Proc *src) {
	IOPorts::init(dst);
	return 0;
}

void ProcBase::terminateArch(Proc *p) {
	IOPorts::free(p);
}

size_t ProcBase::getKMemUsage() const {
	/* 1 pagedir, 1 page-table for kernel-stack, 1 kernelstack for each thread */
	return threads.length() + 2;
}
