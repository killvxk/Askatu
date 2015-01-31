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

#include <arch/mmix/mem/addrspace.h>
#include <assert.h>
#include <common.h>

/*
 * the basic idea here is to give each process a different address-space (means: the number in rV
 * and all PTEs and PTPs), as long it is possible. As soon as 1024 processes are running, the next
 * one has to reuse an existing address-space. This implies, that we have to clear the TCs whenever
 * we switch to a thread of the processes that have an address-space that is used by more than one
 * process. I think its better to spread the processes over the address-spaces instead of putting
 * all additional ones into one address-space. Because if two processes have a similar usage-count,
 * the number of useless TC-clears should be less than if two processes with a different usage-count
 * are put together into one address-space. So, the ideal system would be to put processes with a
 * similar usage-count together, but its not that easy to figure that out in advance. Therefore,
 * we spread the processes over the address-spaces and hope for the best :)
 */

AddressSpace AddressSpace::addrSpaces[ADDR_SPACE_COUNT];
AddressSpace *AddressSpace::freeList = NULL;
AddressSpace *AddressSpace::usedList = NULL;
AddressSpace *AddressSpace::lastUsed = NULL;

void AddressSpace::init() {
	freeList = addrSpaces;
	freeList->next = NULL;
	for(size_t i = 1; i < ADDR_SPACE_COUNT; i++) {
		addrSpaces[i].no = i;
		addrSpaces[i].next = freeList;
		freeList = addrSpaces + i;
	}
}

AddressSpace *AddressSpace::alloc() {
	AddressSpace *as = NULL;
	if(freeList != NULL) {
		/* remove the first of the freelist */
		as = freeList;
		freeList = freeList->next;
	}
	else {
		as = usedList;
		assert(as != NULL);
		/* remove from used-list */
		usedList = usedList->next;
	}

	/* append to used-list */
	as->next = NULL;
	if(lastUsed)
		lastUsed->next = as;
	else
		usedList = as;
	lastUsed = as;
	as->refCount++;
	return as;
}

void AddressSpace::free(AddressSpace *aspace) {
	if(--aspace->refCount == 0) {
		/* remove from usedlist */
		AddressSpace *p = NULL,*as = usedList;
		while(as != NULL) {
			if(as == aspace) {
				if(as == lastUsed)
					lastUsed = p;
				if(p)
					p->next = as->next;
				else
					usedList = as->next;
				break;
			}
			p = as;
			as = as->next;
		}

		/* put on freelist */
		aspace->next = freeList;
		freeList = aspace;
	}
}
