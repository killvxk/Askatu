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

#include <info/link.h>
#include <assert.h>
#include <fstream>

using namespace std;

namespace info {
	std::vector<link*> link::get_list() {
		std::vector<link*> list;
		ifstream f("/sys/net/links");
		while(f.good()) {
			link *l = new link;
			f >> *l;
			if(!f.good()) {
				delete l;
				break;
			}
			list.push_back(l);
		}
		return list;
	}

	istream& operator >>(istream& is,link& l) {
		istream::size_type unlimited = numeric_limits<streamsize>::max();
		uint status = 0;
		is >> l._name >> status >> l._mac >> l._ip >> l._subnetmask >> l._mtu;
		is >> l._rxpkts >> l._txpkts >> l._rxbytes >> l._txbytes;
		is.ignore(unlimited,'\n');
		l._status = static_cast<ask::Net::Status>(status);
		return is;
	}

	std::ostream& operator <<(std::ostream& os,const link& l) {
		os << "Link[name=" << l.name() << ", MAC=" << l.mac()
		   << ", ip=" << l.ip() << ", netmask=" << l.subnetMask() << "]";
		return os;
	}
}
