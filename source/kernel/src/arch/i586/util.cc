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

#include <ask/ipc/ipcbuf.h>
#include <mem/cache.h>
#include <mem/pagedir.h>
#include <mem/physmem.h>
#include <mem/virtmem.h>
#include <sys/keycodes.h>
#include <sys/messages.h>
#include <task/proc.h>
#include <task/smp.h>
#include <vfs/openfile.h>
#include <vfs/vfs.h>
#include <common.h>
#include <cpu.h>
#include <errno.h>
#include <interrupts.h>
#include <ksymbols.h>
#include <log.h>
#include <stdarg.h>
#include <string.h>
#include <util.h>
#include <video.h>

void Util::printUserStateOf(OStream &os,const Thread *t) {
	if(t->getIntrptStack()) {
		frameno_t frame = t->getProc()->getPageDir()->getFrameNo(t->getKernelStack());
		uintptr_t kstackAddr = PageDir::getAccess(frame);
		size_t kstackOff = (uintptr_t)t->getIntrptStack() & (PAGE_SIZE - 1);
		IntrptStackFrame *kstack = (IntrptStackFrame*)(kstackAddr + kstackOff);
		os.writef("User-Register:\n");
		os.writef("\teax=%#08x, ebx=%#08x, ecx=%#08x, edx=%#08x\n",
				kstack->eax,kstack->ebx,kstack->ecx,kstack->edx);
		os.writef("\tesi=%#08x, edi=%#08x, esp=%#08x, ebp=%#08x\n",
				kstack->esi,kstack->edi,kstack->getSP(),kstack->ebp);
		os.writef("\teip=%#08x, eflags=%#08x\n",kstack->getIP(),kstack->getFlags());
		os.writef("\tcr0=%#08x, cr2=%#08x, cr3=%#08x, cr4=%#08x\n",
				CPU::getCR0(),CPU::getCR2(),CPU::getCR3(),CPU::getCR4());
		PageDir::removeAccess(frame);
	}
}

void Util::printUserState(OStream &os) {
	const Thread *t = Thread::getRunning();
	printUserStateOf(os,t);
}
