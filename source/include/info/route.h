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

#include <ask/proto/net.h>
#include <ask/proto/nic.h>
#include <limits>
#include <stddef.h>
#include <string>
#include <vector>

namespace info {
	class route;
	std::istream& operator >>(std::istream& is,route& l);
	std::ostream& operator <<(std::ostream& os,const route& l);

	class route {
		friend std::istream& operator >>(std::istream& is,route& l);
	public:
		static std::vector<route*> get_list();

		explicit route() : _dest(), _subnetmask(), _gw(), _flags(), _link() {
		}

		const ask::Net::IPv4Addr &dest() const {
			return _dest;
		}
		const ask::Net::IPv4Addr &subnetMask() const {
			return _subnetmask;
		}
		const ask::Net::IPv4Addr &gateway() const {
			return _gw;
		}
		uint flags() const {
			return _flags;
		}
		const std::string &link() const {
			return _link;
		}

	private:
		ask::Net::IPv4Addr _dest;
		ask::Net::IPv4Addr _subnetmask;
		ask::Net::IPv4Addr _gw;
		uint _flags;
		std::string _link;
	};
}
