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

#include <ask/col/slist.h>
#include <task/thread.h>
#include <common.h>

/* the IPIs we can send */
#define IPI_WORK			51
#define IPI_FLUSH_TLB		52
#define IPI_WAIT			53
#define IPI_HALT			54
#define IPI_FLUSH_TLB_ACK	55
#define IPI_CALLBACK		56

class Sched;
class OStream;

class SMPBase {
	friend class Sched;
	friend class ThreadBase;

	SMPBase() = delete;

public:
	typedef void (*callback_func)();

	struct CPU : public ask::SListItem {
		explicit CPU(uint8_t id,bool bootstrap,uint8_t ready)
			: ask::SListItem(), id(id), bootstrap(bootstrap), ready(ready), curCycles(), lastCycles(),
			  lastTotal(), lastUpdate(), callback(), thread() {
		}

		uint8_t id;
		uint8_t bootstrap;
		uint8_t ready;
		uint64_t curCycles;
		uint64_t lastCycles;
		uint64_t lastTotal;
		uint64_t lastUpdate;
		callback_func callback;
		Thread *thread;
	};

	typedef ask::SList<CPU>::iterator iterator;

	/**
	 * Inits the SMP-module
	 */
	static void init();

	/**
	 * Starts the SMP-system, i.e. the other CPUs
	 */
	static void start();

	/**
	 * Sets CPU with id <id> to ready
	 *
	 * @param id the CPU-id
	 */
	static void setReady(cpuid_t id);

	/**
	 * Adds the given CPU to the CPU-list
	 *
	 * @param bootstrap whether its the bootstrap CPU
	 * @param id its CPU-id
	 * @param ready whether its ready
	 */
	static void addCPU(bool bootstrap,uint8_t id,uint8_t ready);

	/**
	 * Disables SMP. This is possible even after some CPUs have been added.
	 */
	static void disable();

	/**
	 * @return whether SMP is enabled, i.e. there are multiple CPUs/cores
	 */
	static bool isEnabled() {
		return enabled;
	}

	/**
	 * Updates the runtimes of all CPUs
	 */
	static void updateRuntimes();

	/**
	 * Pauses all other CPUs until SMP::resumeOthers() is called.
	 */
	static void pauseOthers();

	/**
	 * Makes all other CPUs resume their computation
	 */
	static void resumeOthers();

	/**
	 * Halts all other CPUs
	 */
	static void haltOthers();

	/**
	 * Tells all other CPUs to flush their TLB and waits until they have done that
	 */
	static void ensureTLBFlushed();

	/**
	 * Wakes up another CPU, so that it can run a thread
	 */
	static void wakeupCPU();

	/**
	 * If there is any CPU that uses the given pagedir, it is flushed
	 *
	 * @param pdir the pagedir
	 */
	static void flushTLB(PageDir *pdir);

	/**
	 * Calls the callback for CPU <id>
	 *
	 * @param id the CPU-id
	 */
	static void callback(cpuid_t id);

	/**
	 * Let all other CPUs call the given callback
	 *
	 * @param callback the function to call
	 */
	static void callbackOthers(callback_func callback);

	/**
	 * Sends the IPI <vector> to the CPU <id>
	 *
	 * @param id the CPU-id
	 * @param vector the IPI-number
	 */
	static void sendIPI(cpuid_t id,uint8_t vector);

	/**
	 * Renames CPU <old> to CPU <new>
	 *
	 * @param old the old CPU-id
	 * @param newid the new CPU-id
	 */
	static void setId(cpuid_t old,cpuid_t newid);

	/**
	 * @return true if we're executing on the BSP
	 */
	static bool isBSP();

	/**
	 * @return the id of the current CPU
	 */
	static cpuid_t getCurId();

	/**
	 * @return the CPU count
	 */
	static size_t getCPUCount() {
		return cpuCount;
	}

	/**
	 * @return the begin/end of the CPU-list
	 */
	static iterator begin() {
		return cpuList.begin();
	}
	static iterator end() {
		return cpuList.end();
	}

	/**
	 * Prints all CPUs
	 *
	 * @param os the output-stream
	 */
	static void print(OStream &os);

private:
	/**
	 * Architecture-specific initialization. Don't call it!
	 *
	 * @return true on success
	 */
	static bool initArch();

	/**
	 * Should be called if thread <new> is scheduled on CPU <id>
	 *
	 * @param id the CPU-id
	 * @param n the thread to run
	 * @param timestamp the current timestamp
	 */
	static void schedule(cpuid_t id,Thread *n,uint64_t timestamp) {
		CPU *c = cpus[id];
		if(EXPECT_TRUE(c->thread && !(c->thread->getFlags() & T_IDLE)))
			c->curCycles += timestamp - c->thread->getStats().cycleStart;
		c->thread = n;
	}

	static CPU *getCPUById(cpuid_t id);

	static bool enabled;
	static ask::SList<CPU> cpuList;
	static CPU **cpus;
	static size_t cpuCount;
};

#if defined(__x86__)
#	include <arch/x86/task/smp.h>
#elif defined __eco32__
#	include <arch/eco32/task/smp.h>
#else
#	include <arch/mmix/task/smp.h>
#endif

inline void SMPBase::setReady(cpuid_t id) {
	assert(cpus[id]);
	cpus[id]->ready = true;
}

inline bool SMPBase::isBSP() {
	cpuid_t cur = getCurId();
	return cpus[cur]->bootstrap;
}
