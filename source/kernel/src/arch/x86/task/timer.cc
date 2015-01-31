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

#include <arch/x86/ioapic.h>
#include <arch/x86/lapic.h>
#include <arch/x86/pic.h>
#include <arch/x86/pit.h>
#include <arch/x86/ports.h>
#include <arch/x86/rtc.h>
#include <task/smp.h>
#include <task/timer.h>
#include <common.h>
#include <config.h>
#include <cpu.h>
#include <log.h>

uint64_t Timer::bootTSC = 0;
time_t Timer::bootTime = 0;
uint64_t Timer::cpuMhz;

void TimerBase::archInit() {
	Timer::bootTSC = CPU::rdtsc();
	Timer::bootTime = RTC::getTime();
}

void Timer::start(bool isBSP) {
	if(!Config::get(Config::FORCE_PIT) && LAPIC::isAvailable()) {
		Log::get().writef("CPU %d uses LAPIC as timer device\n",SMP::getCurId());
		if(isBSP) {
			/* mask it as well */
			if(IOAPIC::enabled())
				IOAPIC::mask(IOAPIC::irqToGsi(Interrupts::IRQ_PIT - Interrupts::IRQ_MASTER_BASE));
			else
				PIC::mask(Interrupts::IRQ_PIT - Interrupts::IRQ_MASTER_BASE);
		}
		LAPIC::enableTimer();
	}
	else if(isBSP) {
		Log::get().writef("CPU %d uses PIT as timer device\n",SMP::getCurId());
		if(SMP::getCPUCount() > 1)
			Log::get().writef("WARNING: using PIT with multiple CPUs means no preemption on APs!\n");
		/* change timer divisor */
		uint freq = PIT::BASE_FREQUENCY / Timer::FREQUENCY_DIV;
		PIT::enablePeriodic(PIT::CHAN0,freq);
	}
}

void Timer::wait(uint us) {
	uint64_t start = CPU::rdtsc();
	uint64_t end = start + timeToCycles(us);
	while(CPU::rdtsc() < end)
		CPU::pause();
}

uint64_t Timer::detectCPUSpeed(uint64_t *busHz) {
	int bestMatches = -1;
	uint64_t bestHz = 0;
	uint64_t bestBusHz = 0;
	for(int i = 1; i <= MEASURE_COUNT; i++) {
		bool found = true;
		uint64_t ref = determineSpeed(0x1000 * i * 10,busHz);
		/* no tick at all? */
		if(ref == 0)
			continue;

		int j;
		for(j = 0; j < REQUIRED_MATCHES; j++) {
			uint64_t hz = determineSpeed(0x1000 * i * 10,busHz);
			/* not in tolerance? */
			if(((ref - hz) > 0 && (ref - hz) > TOLERANCE) ||
				((hz - ref) > 0 && (hz - ref) > TOLERANCE)) {
				found = false;
				break;
			}
		}
		if(found) {
			bestHz = ref;
			bestBusHz = *busHz;
			break;
		}
		/* store our best result */
		if(j > bestMatches) {
			bestMatches = j;
			bestHz = ref;
			bestBusHz = *busHz;
		}
	}
	/* ok give up and use the best result so far */
	cpuMhz = bestHz / 1000000;
	*busHz = bestBusHz;
	return bestHz;
}

uint64_t Timer::determineSpeed(int instrCount,uint64_t *busHz) {
	/* set PIT and LAPIC counter to -1 */
	PIT::enablePeriodic(PIT::CHAN0,0xFFFF);
	uint32_t pitBegin = PIT::getCounter(PIT::CHAN0);
	uint32_t lapicBegin = 0;
	if(LAPIC::isAvailable()) {
		LAPIC::setTimer(0xFFFFFFFF);
		/* better read the value again. perhaps it has not been set */
		lapicBegin = LAPIC::getTimer();
	}

	/* now spend some time and measure the number of cycles */
	uint64_t before = CPU::rdtsc();
	for(volatile int i = instrCount; i > 0; i--)
		;
	uint64_t after = CPU::rdtsc();

	/* read current counts */
	uint32_t left = PIT::getCounter(PIT::CHAN0);
	uint32_t lapicCnt = 0;
	if(LAPIC::isAvailable())
		lapicCnt = LAPIC::getTimer();
	/* if there was no tick at all, we have to increase instrCount */
	if(left == pitBegin)
		return 0;

	/* calculate frequencies */
	ulong picTime = PIT::BASE_FREQUENCY / (pitBegin - left);
	if(LAPIC::isAvailable())
		*busHz = (lapicBegin - lapicCnt) * picTime * LAPIC::TIMER_DIVIDER;
	return (after - before) * picTime;
}
