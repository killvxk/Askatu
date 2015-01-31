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

#include <sys/test.h>
#include <task/sched.h>
#include <common.h>

/* forward declarations */
static void test_sched();

/* our test-module */
sTestModule tModSched = {
	"Scheduling",
	&test_sched
};

static void test_sched() {
	/* TODO implement for threads */
#if 0
	Proc *x = (Proc*)Proc::getByPid(Proc::getFreePid());
	Proc *rand[5] = {x + 1,x,x + 4,x + 2,x + 3};
	size_t i;
	bool res = true;
	/* not relevant here */
	Proc *p = (Proc*)x, *pp;

	test_caseStart("Enqueuing - dequeuing");
	/* enqueue */
	for(i = 0; i < PROC_COUNT - 1; i++) {
		sched_setReady(p++);
	}
	/* dequeue */
	pp = (Proc*)x;
	while((p = sched_dequeueReady()) != NULL) {
		p->state = Thread::UNUSED;
		if(p != pp) {
			res = false;
			break;
		}
		pp++;
	}
	if(res)
		test_caseSucceeded();
	else
		test_caseFailed("Got process 0x%x, expected 0x%x",p,pp);

	/* second test */
	res = true;
	test_caseStart("Dequeue specific processes (opposite order)");
	/* enqueue some processes */
	p = (Proc*)x;
	for(i = 0; i < 5; i++)
		sched_setReady(p++);
	/* dequeue */
	p--;
	for(i = 4; i >= 0; i--) {
		res = res && sched_dequeueReadyProc(p);
		res = res && !sched_dequeueReadyProc(p);
		p->state = Thread::UNUSED;
		p--;
	}
	if(res)
		test_caseSucceeded();
	else
		test_caseFailed("");

	/* third test */
	res = true;
	test_caseStart("Dequeue specific processes (random order)");
	/* enqueue some processes */
	p = (Proc*)x;
	for(i = 0; i < 5; i++)
		sched_setReady(p++);
	/* dequeue */
	for(i = 0; i < 5; i++) {
		res = res && sched_dequeueReadyProc(rand[i]);
		res = res && !sched_dequeueReadyProc(rand[i]);
		rand[i]->state = Thread::UNUSED;
	}
	if(res)
		test_caseSucceeded();
	else
		test_caseFailed("");
#endif
}
