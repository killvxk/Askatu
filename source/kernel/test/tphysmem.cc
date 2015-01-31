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

#include <mem/physmem.h>
#include <sys/test.h>
#include <common.h>

#include "testutils.h"

#define FRAME_COUNT 50

/* forward declarations */
static void test_mm();
static void test_default();
static void test_contiguous();
static void test_contiguous_align();
static void test_mm_allocate();
static void test_mm_free();

/* our test-module */
sTestModule tModMM = {
	"Physical memory-management",
	&test_mm
};

static frameno_t frames[FRAME_COUNT];

static void test_mm() {
	test_default();
	test_contiguous();
	test_contiguous_align();
}

static void test_default() {
	test_caseStart("Requesting and freeing %d frames",FRAME_COUNT);

	checkMemoryBefore(false);
	test_mm_allocate();
	test_mm_free();
	checkMemoryAfter(false);

	test_caseSucceeded();
}

static void test_contiguous() {
	ssize_t res1,res2,res3,res4;

	test_caseStart("Requesting once and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(3,1);
	PhysMem::freeContiguous(res1,3);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("Requesting twice and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(6,1);
	res2 = PhysMem::allocateContiguous(5,1);
	PhysMem::freeContiguous(res1,6);
	PhysMem::freeContiguous(res2,5);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("Request, free, request and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(5,1);
	res2 = PhysMem::allocateContiguous(5,1);
	res3 = PhysMem::allocateContiguous(5,1);
	PhysMem::freeContiguous(res2,5);
	res2 = PhysMem::allocateContiguous(3,1);
	res4 = PhysMem::allocateContiguous(3,1);
	PhysMem::freeContiguous(res1,5);
	PhysMem::freeContiguous(res2,3);
	PhysMem::freeContiguous(res3,5);
	PhysMem::freeContiguous(res4,3);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("Request a lot multiple times and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(35,1);
	res2 = PhysMem::allocateContiguous(12,1);
	res3 = PhysMem::allocateContiguous(89,1);
	res4 = PhysMem::allocateContiguous(56,1);
	PhysMem::freeContiguous(res3,89);
	PhysMem::freeContiguous(res1,35);
	PhysMem::freeContiguous(res2,12);
	PhysMem::freeContiguous(res4,56);
	checkMemoryAfter(false);
	test_caseSucceeded();
}

static void test_contiguous_align() {
	ssize_t res1,res2,res3,res4;

	test_caseStart("[Align] Requesting once and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(3,4);
	test_assertTrue((res1 % 4) == 0);
	PhysMem::freeContiguous(res1,3);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("[Align] Requesting twice and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(6,4);
	test_assertTrue((res1 % 4) == 0);
	res2 = PhysMem::allocateContiguous(5,8);
	test_assertTrue((res2 % 8) == 0);
	PhysMem::freeContiguous(res1,6);
	PhysMem::freeContiguous(res2,5);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("[Align] Request, free, request and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(5,16);
	test_assertTrue((res1 % 16) == 0);
	res2 = PhysMem::allocateContiguous(5,16);
	test_assertTrue((res2 % 16) == 0);
	res3 = PhysMem::allocateContiguous(5,16);
	test_assertTrue((res3 % 16) == 0);
	PhysMem::freeContiguous(res2,5);
	res2 = PhysMem::allocateContiguous(3,64);
	test_assertTrue((res2 % 64) == 0);
	res4 = PhysMem::allocateContiguous(3,64);
	test_assertTrue((res4 % 64) == 0);
	PhysMem::freeContiguous(res1,5);
	PhysMem::freeContiguous(res2,3);
	PhysMem::freeContiguous(res3,5);
	PhysMem::freeContiguous(res4,3);
	checkMemoryAfter(false);
	test_caseSucceeded();

	test_caseStart("[Align] Request a lot multiple times and free");
	checkMemoryBefore(false);
	res1 = PhysMem::allocateContiguous(35,4);
	test_assertTrue((res1 % 4) == 0);
	res2 = PhysMem::allocateContiguous(12,4);
	test_assertTrue((res2 % 4) == 0);
	res3 = PhysMem::allocateContiguous(89,4);
	test_assertTrue((res3 % 4) == 0);
	res4 = PhysMem::allocateContiguous(56,4);
	test_assertTrue((res4 % 4) == 0);
	PhysMem::freeContiguous(res3,89);
	PhysMem::freeContiguous(res1,35);
	PhysMem::freeContiguous(res2,12);
	PhysMem::freeContiguous(res4,56);
	checkMemoryAfter(false);
	test_caseSucceeded();
}

static void test_mm_allocate() {
	ssize_t i = 0;
	while(i < FRAME_COUNT) {
		frames[i] = PhysMem::allocate(PhysMem::KERN);
		i++;
	}
}

static void test_mm_free() {
	ssize_t i = FRAME_COUNT - 1;
	while(i >= 0) {
		PhysMem::free(frames[i],PhysMem::KERN);
		i--;
	}
}
