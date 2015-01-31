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

#include <ask/proto/socket.h>
#include <sys/common.h>
#include <bitset>
#include <stdlib.h>

template<size_t N>
class PortMng {
public:
	explicit PortMng(ask::port_t base) : _base(base), _free(N), _ports() {
	}

	ask::port_t allocate() {
		// TODO handle that case
		assert(_free > 0);
		ask::port_t p;
		do {
			p = rand() % N;
		}
		while(_ports[p]);
		_ports[p] = true;
		_free--;
		return _base + p;
	}
	void release(ask::port_t port) {
		assert(port >= _base && port < _base + N);
		_ports[port - _base] = false;
		_free++;
	}

private:
	ask::port_t _base;
	size_t _free;
	std::bitset<N> _ports;
};
