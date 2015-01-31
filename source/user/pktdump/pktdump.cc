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

#include <ask/proto/nic.h>
#include <ask/proto/socket.h>
#include <sys/common.h>

using namespace ask;

struct EthernetHeader {
	NIC::MAC dst;
	NIC::MAC src;
	uint16_t type;
};

int main() {
	ulong buffer[1024];
	Socket sock("/dev/socket",Socket::SOCK_RAW_ETHER,Socket::PROTO_ANY);
	while(1) {
		ask::Socket::Addr addr;
		size_t res = sock.recvfrom(addr,buffer,sizeof(buffer));

		EthernetHeader *ether = reinterpret_cast<EthernetHeader*>(buffer);
		std::cout << "Got packet:\n";
		std::cout << " src  = " << ether->src << "\n";
		std::cout << " dst  = " << ether->dst << "\n";
		std::cout << " type = " << ether->type << "\n";
		std::cout << " len  = " << res << "\n";
		std::cout << std::endl;
	}
	return 0;
}
