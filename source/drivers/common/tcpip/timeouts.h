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
#include <functor.h>
#include <list>
#include <mutex>

class Timeouts {
	Timeouts() = delete;

public:
	typedef std::Functor<void> callback_type;

	struct Entry {
		explicit Entry(int _id,callback_type *_cb,uint _timestamp)
			: id(_id), cb(_cb), timestamp(_timestamp) {
		}

		int id;
		callback_type *cb;
		uint timestamp;
	};

	static int thread(void*);

	static int allocateId() {
		return _nextId++;
	}

	static void program(int id,callback_type *cb,uint msecs);
	static void cancel(int id);

private:
	static uint _now;
	static int _nextId;
	static std::list<Entry> _list;
};
