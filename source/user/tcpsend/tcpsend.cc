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

#include <ask/proto/net.h>
#include <ask/proto/socket.h>
#include <sys/cmdargs.h>
#include <sys/common.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

using namespace ask;

static char buffer[8192];

static void usage(const char *name) {
	fprintf(stderr,"Usage: %s <file> <ip> <port>\n",name);
	exit(EXIT_FAILURE);
}

int main(int argc,char **argv) {
	if(isHelpCmd(argc,argv) || argc != 4)
		usage(argv[0]);

	ask::Socket::Addr addr;
	addr.family = ask::Socket::AF_INET;
	Socket sock("/dev/socket",Socket::SOCK_STREAM,Socket::PROTO_TCP);

	ask::Net::IPv4Addr ip;
	std::istringstream is(argv[2]);
	is >> ip;

	addr.d.ipv4.addr = ip.value();
	addr.d.ipv4.port = atoi(argv[3]);
	sock.connect(addr);

	int fd = open(argv[1],O_RDONLY);
	if(fd < 0)
		error("Unable to open '%s' for reading",argv[1]);

	/* send size first */
	uint32_t size = filesize(fd);
	sock.send(&size,sizeof(size));

	/* send file */
	ssize_t res;
	while((res = read(fd,buffer,sizeof(buffer))) > 0)
		sock.send(buffer,res);
	close(fd);
	return 0;
}
