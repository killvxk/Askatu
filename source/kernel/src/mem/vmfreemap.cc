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

#include <mem/cache.h>
#include <mem/vmfreemap.h>
#include <assert.h>
#include <common.h>
#include <video.h>

/**
 * The idea here is to allow a quick allocation and release of areas in the free-area of the
 * virtual address space (for shared mem, shared libraries, TLS, ...). Therefore, we use a
 * malloc-like management that keeps track of the free areas (not the used ones).
 */

bool VMFreeMap::init(VMFreeMap *map,uintptr_t addr,size_t size) {
	Area *a = (Area*)Cache::alloc(sizeof(Area));
	if(!a)
		return false;
	a->addr = addr;
	a->size = size;
	a->next = NULL;
	map->list = a;
	return true;
}

void VMFreeMap::destroy() {
	for(Area *a = list; a != NULL; ) {
		Area *n = a->next;
		Cache::free(a);
		a = n;
	}
	list = NULL;
}

uintptr_t VMFreeMap::allocate(size_t size) {
	Area *a;
	/* TODO is that correct on archs with page-size != 0x1000? */
	assert((size & 0xFFF) == 0);
	Area *p = NULL;
	for(a = list; a != NULL; p = a, a = a->next) {
		if(a->size >= size)
			break;
	}
	if(a == NULL)
		return 0;

	/* take it from the front */
	uintptr_t res = a->addr;
	a->size -= size;
	a->addr += size;
	/* if the area is empty now, remove it */
	if(a->size == 0) {
		if(p)
			p->next = a->next;
		else
			list = a->next;
		Cache::free(a);
	}
	return res;
}

bool VMFreeMap::allocateAt(uintptr_t addr,size_t size) {
	Area *a,*p = NULL;
	for(a = list; a != NULL && addr > a->addr + a->size; p = a, a = a->next)
		;
	/* invalid position or too small? */
	if(a == NULL || addr + size > a->addr + a->size || (!p && addr < a->addr))
		return false;

	/* complete area? */
	if(addr == a->addr && size == a->size) {
		if(p)
			p->next = a->next;
		else
			list = a->next;
		Cache::free(a);
	}
	/* at the beginning? */
	else if(addr == a->addr) {
		a->addr += size;
		a->size -= size;
	}
	/* at the end? */
	else if(addr + size == a->addr + a->size) {
		a->size -= size;
	}
	/* in the middle */
	else {
		Area *na = (Area*)Cache::alloc(sizeof(Area));
		if(!na)
			return false;
		na->addr = a->addr;
		na->size = addr - a->addr;
		na->next = a;
		if(p)
			p->next = na;
		else
			list = na;
		a->addr = addr + size;
		a->size -= na->size + size;
	}
	return true;
}

void VMFreeMap::free(uintptr_t addr,size_t size) {
	assert((size & 0xFFF) == 0);
	/* find the area behind ours */
	Area *n,*p = NULL;
	for(n = list; n != NULL && addr > n->addr; p = n, n = n->next)
		;

	/* merge with prev and next */
	if(p && p->addr + p->size == addr && n && addr + size == n->addr) {
		p->size += size + n->size;
		p->next = n->next;
		Cache::free(n);
	}
	/* merge with prev */
	else if(p && p->addr + p->size == addr) {
		p->size += size;
	}
	/* merge with next */
	else if(n && addr + size == n->addr) {
		n->addr -= size;
		n->size += size;
	}
	/* create new area between them */
	else {
		Area *a = (Area*)Cache::alloc(sizeof(Area));
		/* if this fails, ignore it; we can't really do something about it */
		if(!a)
			return;
		a->addr = addr;
		a->size = size;
		if(p)
			p->next = a;
		else
			list = a;
		a->next = n;
	}
}

size_t VMFreeMap::getSize(size_t *areas) const {
	size_t total = 0;
	*areas = 0;
	for(Area *a = list; a != NULL; a = a->next) {
		total += a->size;
		(*areas)++;
	}
	return total;
}

void VMFreeMap::print(OStream &os) const {
	size_t areas;
	os.writef("Free area with %zu MiB:\n",getSize(&areas) / (1024 * 1024));
	for(Area *a = list; a != NULL; a = a->next)
		os.writef("\t@ %p, %zu MiB\n",a->addr,a->size / (1024 * 1024));
}
