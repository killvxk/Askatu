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

#include <sys/common.h>
#include <sys/conf.h>
#include <sys/mman.h>
#include <assert.h>
#include <iterator>
#include <new>
#include <vector>

/**
 * A container for the lines in less. The basic idea is not to use the heap, but use chgsize()
 * manually to add new pages to the end if necessary and use it as a big array of lines, where
 * each line has the same length. Because the heap uses chgsize() as well, we can't use a single
 * expanding array, but have to use multiple ones. Thus, we use a region-based concept.
 */
class LineContainer {
public:
	typedef size_t size_type;

private:
	/**
	 * A region, i.e. a dynamically expanding array
	 */
	class Region {
	public:
		typedef LineContainer::size_type size_type;

		Region()
			: _begin((uintptr_t)chgsize(0)), _pages(0), _lines(0) {
		}
		~Region() {
		}

		bool append(size_type size,const char *line);
		char *get(size_type size,size_type i) const;

		size_type lines() const {
			return _lines;
		}

	private:
		uintptr_t _begin;
		size_type _pages;
		size_type _lines;
	};

public:
	LineContainer(size_type lineLen)
		// plus null-termination
		: _regions(std::vector<Region>()), _lines(0), _lineLen(lineLen + 1) {
		_regions.push_back(Region());
	}
	~LineContainer() {
	}

	const char *get(size_type index);
	void append(const char *line);
	size_type size() const {
		return _lines;
	}

private:
	std::vector<Region> _regions;
	size_type _lines;
	size_type _lineLen;
};
