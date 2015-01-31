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
#include <vector>

#include "common.h"
#include "link.h"

class Route {
	explicit Route() : dest(), netmask(), gateway(), flags(), link() {
	}

public:
	explicit Route(const ask::Net::IPv4Addr &dst,const ask::Net::IPv4Addr &nm,
			const ask::Net::IPv4Addr &gw,uint fl,const std::shared_ptr<Link> &l)
		: dest(dst), netmask(nm), gateway(gw), flags(fl), link(l) {
	}

	bool valid() const {
		return flags != 0;
	}

	static int insert(const ask::Net::IPv4Addr &ip,const ask::Net::IPv4Addr &nm,
		const ask::Net::IPv4Addr &gw,uint flags,const std::shared_ptr<Link> &l);
	static Route find(const ask::Net::IPv4Addr &ip);
	static int setStatus(const ask::Net::IPv4Addr &ip,ask::Net::Status status);
	static int remove(const ask::Net::IPv4Addr &ip);
	static void removeAll(const std::shared_ptr<Link> &link);
	static void print(std::ostream &os);

	ask::Net::IPv4Addr dest;
	ask::Net::IPv4Addr netmask;
	ask::Net::IPv4Addr gateway;
	uint flags;
	std::shared_ptr<Link> link;

private:
	static std::mutex _mutex;
	static std::vector<Route*> _table;
};
