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

#include <ask/ipc/filedev.h>
#include <ask/dns.h>
#include <sys/common.h>
#include <sys/driver.h>
#include <sys/sync.h>
#include <sys/thread.h>
#include <sys/time.h>
#include <usergroup/group.h>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stdio.h>
#include <vector>

#include "proto/arp.h"
#include "proto/ethernet.h"
#include "proto/icmp.h"
#include "proto/udp.h"
#include "socket/dgramsocket.h"
#include "socket/rawethersock.h"
#include "socket/rawipsock.h"
#include "socket/streamsocket.h"
#include "link.h"
#include "linkmng.h"
#include "packet.h"
#include "route.h"
#include "timeouts.h"

std::mutex mutex;

static int receiveThread(void *arg);

class SocketDevice : public ask::ClientDevice<Socket> {
public:
	explicit SocketDevice(const char *path,mode_t mode)
		: ask::ClientDevice<Socket>(path,mode,DEV_TYPE_BLOCK,
			DEV_OPEN | DEV_CANCEL | DEV_SHFILE | DEV_CREATSIBL | DEV_READ | DEV_WRITE | DEV_CLOSE) {
		set(MSG_FILE_OPEN,std::make_memfun(this,&SocketDevice::open));
		set(MSG_FILE_READ,std::make_memfun(this,&SocketDevice::read));
		set(MSG_FILE_WRITE,std::make_memfun(this,&SocketDevice::write));
		set(MSG_FILE_CLOSE,std::make_memfun(this,&SocketDevice::close),false);
		set(MSG_DEV_CANCEL,std::make_memfun(this,&SocketDevice::cancel));
		set(MSG_DEV_CREATSIBL,std::make_memfun(this,&SocketDevice::creatsibl));
		set(MSG_SOCK_CONNECT,std::make_memfun(this,&SocketDevice::connect));
		set(MSG_SOCK_BIND,std::make_memfun(this,&SocketDevice::bind));
		set(MSG_SOCK_LISTEN,std::make_memfun(this,&SocketDevice::listen));
		set(MSG_SOCK_RECVFROM,std::make_memfun(this,&SocketDevice::recvfrom));
		set(MSG_SOCK_SENDTO,std::make_memfun(this,&SocketDevice::sendto));
		set(MSG_SOCK_ABORT,std::make_memfun(this,&SocketDevice::abort));
	}

	void open(ask::IPCStream &is) {
		char buffer[MAX_PATH_LEN];
		ask::FileOpen::Request r(buffer,sizeof(buffer));
		is >> r;

		int type = 0, proto = 0;
		std::istringstream sip(r.path.str());
		sip >> type >> proto;

		int res = 0;
		{
			std::lock_guard<std::mutex> guard(mutex);
			switch(type) {
				case ask::Socket::SOCK_DGRAM:
					add(is.fd(),new DGramSocket(is.fd(),proto));
					break;
				case ask::Socket::SOCK_STREAM:
					add(is.fd(),new StreamSocket(is.fd(),proto));
					break;
				case ask::Socket::SOCK_RAW_ETHER:
					add(is.fd(),new RawEtherSocket(is.fd(),proto));
					break;
				case ask::Socket::SOCK_RAW_IP:
					add(is.fd(),new RawIPSocket(is.fd(),proto));
					break;
				default:
					res = -ENOTSUP;
					break;
			}
		}

		is << ask::FileOpen::Response(res) << ask::Reply();
	}

	void connect(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		ask::Socket::Addr sa;
		is >> sa;

		int res;
		{
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->connect(&sa,is.msgid());
		}
		if(res < 0)
			is << res << ask::Reply();
	}

	void bind(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		ask::Socket::Addr sa;
		is >> sa;

		int res;
		{
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->bind(&sa);
		}
		is << res << ask::Reply();
	}

	void listen(ask::IPCStream &is) {
		Socket *sock = get(is.fd());

		int res;
		{
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->listen();
		}
		is << res << ask::Reply();
	}

	void cancel(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		msgid_t mid;
		is >> mid;

		int res,msgtype = mid & 0xFFFF;
		if(msgtype != MSG_FILE_WRITE && msgtype != MSG_SOCK_SENDTO &&
				msgtype != MSG_FILE_READ && msgtype != MSG_SOCK_RECVFROM &&
				msgtype != MSG_DEV_CREATSIBL) {
			res = -EINVAL;
		}
		else {
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->cancel(mid);
		}

		is << res << ask::Reply();
	}

	void creatsibl(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		ask::FileCreatSibl::Request r;
		is >> r;

		int res;
		{
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->accept(is.msgid(),r.nfd,this);
		}
		if(res < 0)
			is << ask::FileCreatSibl::Response(res) << ask::Reply();
	}

	void read(ask::IPCStream &is) {
		handleRead(is,false);
	}

	void recvfrom(ask::IPCStream &is) {
		handleRead(is,true);
	}

	void write(ask::IPCStream &is) {
		ask::FileWrite::Request r;
		is >> r;
		handleWrite(is,r,NULL);
	}

	void sendto(ask::IPCStream &is) {
		ask::FileWrite::Request r;
		ask::Socket::Addr sa;
		is >> r >> sa;
		handleWrite(is,r,&sa);
	}

	void abort(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		int res = sock->abort();
		is << res << ask::Reply();
	}

	void close(ask::IPCStream &is) {
		Socket *sock = get(is.fd());
		std::lock_guard<std::mutex> guard(mutex);
		// don't delete it; let the object itself decide when it is destroyed (for TCP)
		remove(is.fd(),false);
		sock->disconnect();
		Device::close(is);
	}

private:
	void handleRead(ask::IPCStream &is,bool needsSockAddr) {
		Socket *sock = get(is.fd());
		ask::FileRead::Request r;
		is >> r;

		ssize_t res;
		{
			// TODO check offset!
			char *data = NULL;
			if(r.shmemoff != -1)
				data = sock->shm() + r.shmemoff;

			std::lock_guard<std::mutex> guard(mutex);
			res = sock->recvfrom(is.msgid(),needsSockAddr,data,r.count);
		}

		if(res < 0)
			is << ask::FileRead::Response(res) << ask::Reply();
	}

	void handleWrite(ask::IPCStream &is,ask::FileWrite::Request &r,const ask::Socket::Addr *sa) {
		Socket *sock = get(is.fd());

		// TODO check offset!
		ask::DataBuf buf(r.count,sock->shm(),r.shmemoff);
		if(r.shmemoff == -1)
			is >> ask::ReceiveData(buf.data(),r.count);

		ssize_t res;
		{
			std::lock_guard<std::mutex> guard(mutex);
			res = sock->sendto(is.msgid(),sa,buf.data(),r.count);
		}

		if(res != 0)
			is << ask::FileWrite::Response(res) << ask::Reply();
	}
};

class NetDevice : public ask::Device {
public:
	explicit NetDevice(const char *path,mode_t mode)
		: ask::Device(path,mode,DEV_TYPE_SERVICE,0) {
		set(MSG_NET_LINK_ADD,std::make_memfun(this,&NetDevice::linkAdd));
		set(MSG_NET_LINK_REM,std::make_memfun(this,&NetDevice::linkRem));
		set(MSG_NET_LINK_CONFIG,std::make_memfun(this,&NetDevice::linkConfig));
		set(MSG_NET_LINK_MAC,std::make_memfun(this,&NetDevice::linkMAC));
		set(MSG_NET_ROUTE_ADD,std::make_memfun(this,&NetDevice::routeAdd));
		set(MSG_NET_ROUTE_REM,std::make_memfun(this,&NetDevice::routeRem));
		set(MSG_NET_ROUTE_CONFIG,std::make_memfun(this,&NetDevice::routeConfig));
		set(MSG_NET_ROUTE_GET,std::make_memfun(this,&NetDevice::routeGet));
		set(MSG_NET_ARP_ADD,std::make_memfun(this,&NetDevice::arpAdd));
		set(MSG_NET_ARP_REM,std::make_memfun(this,&NetDevice::arpRem));
	}

	void linkAdd(ask::IPCStream &is) {
		ask::CStringBuf<Link::NAME_LEN> name;
		ask::CStringBuf<MAX_PATH_LEN> path;
		is >> name >> path;

		std::lock_guard<std::mutex> guard(mutex);
		int res = LinkMng::add(name.str(),path.str());
		if(res == 0) {
			std::shared_ptr<Link> link = LinkMng::getByName(name.str());
			std::shared_ptr<Link> *linkcpy = new std::shared_ptr<Link>(link);
			if((res = startthread(receiveThread,linkcpy)) < 0) {
				LinkMng::rem(name.str());
				delete linkcpy;
			}
		}
		is << res << ask::Reply();
	}

	void linkRem(ask::IPCStream &is) {
		ask::CStringBuf<Link::NAME_LEN> name;
		is >> name;

		std::lock_guard<std::mutex> guard(mutex);
		int res = LinkMng::rem(name.str());
		is << res << ask::Reply();
	}

	void linkConfig(ask::IPCStream &is) {
		ask::CStringBuf<Link::NAME_LEN> name;
		ask::Net::IPv4Addr ip,netmask;
		ask::Net::Status status;
		is >> name >> ip >> netmask >> status;

		std::lock_guard<std::mutex> guard(mutex);
		int res = 0;
		std::shared_ptr<Link> l = LinkMng::getByName(name.str());
		std::shared_ptr<Link> other;
		if(!l)
			res = -ENOTFOUND;
		else if((other = LinkMng::getByIp(ip)) && other != l)
			res = -EEXIST;
		else if(ip.value() != 0 && !ip.isHost(netmask))
			res = -EINVAL;
		else if(!netmask.isNetmask() || status == ask::Net::KILLED)
			res = -EINVAL;
		else {
			l->ip(ip);
			l->subnetMask(netmask);
			l->status(status);
		}
		is << res << ask::Reply();
	}

	void linkMAC(ask::IPCStream &is) {
		ask::CStringBuf<Link::NAME_LEN> name;
		is >> name;

		std::lock_guard<std::mutex> guard(mutex);
		std::shared_ptr<Link> link = LinkMng::getByName(name.str());
		if(!link)
			is << -ENOTFOUND << ask::Reply();
		else
			is << 0 << link->mac() << ask::Reply();
	}

	void routeAdd(ask::IPCStream &is) {
		ask::CStringBuf<Link::NAME_LEN> link;
		ask::Net::IPv4Addr ip,gw,netmask;
		is >> link >> ip >> gw >> netmask;

		std::lock_guard<std::mutex> guard(mutex);
		int res = 0;
		std::shared_ptr<Link> l = LinkMng::getByName(link.str());
		if(!l)
			res = -ENOTFOUND;
		else {
			uint flags = ask::Net::FL_UP;
			if(gw.value() != 0)
				flags |= ask::Net::FL_USE_GW;
			Route::insert(ip,netmask,gw,flags,l);
		}
		is << res << ask::Reply();
	}

	void routeRem(ask::IPCStream &is) {
		ask::Net::IPv4Addr ip;
		is >> ip;

		std::lock_guard<std::mutex> guard(mutex);
		int res = Route::remove(ip);
		is << res << ask::Reply();
	}

	void routeConfig(ask::IPCStream &is) {
		ask::Net::IPv4Addr ip;
		ask::Net::Status status;
		is >> ip >> status;

		std::lock_guard<std::mutex> guard(mutex);
		int res = Route::setStatus(ip,status);
		is << res << ask::Reply();
	}

	void routeGet(ask::IPCStream &is) {
		ask::Net::IPv4Addr ip;
		is >> ip;

		std::lock_guard<std::mutex> guard(mutex);
		Route r = Route::find(ip);
		if(!r.valid())
			is << -ENETUNREACH << ask::Reply();
		else {
			is << 0;
			is << (r.flags & ask::Net::FL_USE_GW ? r.gateway : r.dest);
			is << ask::CString(r.link->name().c_str(),r.link->name().length());
			is << ask::Reply();
		}
	}

	void arpAdd(ask::IPCStream &is) {
		ask::Net::IPv4Addr ip;
		is >> ip;

		std::lock_guard<std::mutex> guard(mutex);
		int res = 0;
		Route route = Route::find(ip);
		if(!route.valid())
			res = -ENOTFOUND;
		else
			ARP::requestMAC(route.link,ip);
		is << res << ask::Reply();
	}

	void arpRem(ask::IPCStream &is) {
		ask::Net::IPv4Addr ip;
		is >> ip;

		std::lock_guard<std::mutex> guard(mutex);
		int res = ARP::remove(ip);
		is << res << ask::Reply();
	}
};

class LinksFileDevice : public ask::FileDevice {
public:
	explicit LinksFileDevice(const char *path,mode_t mode)
		: ask::FileDevice(path,mode) {
	}

	virtual std::string handleRead() {
		std::ostringstream os;
		std::lock_guard<std::mutex> guard(mutex);
		LinkMng::print(os);
		return os.str();
	}
};

class RoutesFileDevice : public ask::FileDevice {
public:
	explicit RoutesFileDevice(const char *path,mode_t mode)
		: ask::FileDevice(path,mode) {
	}

	virtual std::string handleRead() {
		std::ostringstream os;
		std::lock_guard<std::mutex> guard(mutex);
		Route::print(os);
		return os.str();
	}
};

class ARPFileDevice : public ask::FileDevice {
public:
	explicit ARPFileDevice(const char *path,mode_t mode)
		: ask::FileDevice(path,mode) {
	}

	virtual std::string handleRead() {
		std::ostringstream os;
		std::lock_guard<std::mutex> guard(mutex);
		ARP::print(os);
		return os.str();
	}
};

class SocketsFileDevice : public ask::FileDevice {
public:
	explicit SocketsFileDevice(const char *path,mode_t mode)
		: ask::FileDevice(path,mode) {
	}

	virtual std::string handleRead() {
		std::ostringstream os;
		std::lock_guard<std::mutex> guard(mutex);
		TCP::printSockets(os);
		UDP::printSockets(os);
		return os.str();
	}
};

static int receiveThread(void *arg) {
	std::shared_ptr<Link> *linkptr = reinterpret_cast<std::shared_ptr<Link>*>(arg);
	const std::shared_ptr<Link> link = *linkptr;
	uint8_t *buffer = reinterpret_cast<uint8_t*>(link->sharedmem());
	while(link->status() != ask::Net::KILLED) {
		ssize_t res = link->read(buffer,link->mtu());
		if(res < 0) {
			printe("Reading packet failed");
			break;
		}

		if((size_t)res >= sizeof(Ethernet<>)) {
			std::lock_guard<std::mutex> guard(mutex);
			Packet pkt(buffer,res);
			ssize_t err = Ethernet<>::receive(link,pkt);
			if(err < 0)
				std::cerr << "Ignored packet of size " << res << ": " << strerror(err) << "\n";
		}
		else
			printe("Ignoring packet of size %zd",res);
	}
	LinkMng::rem(link->name());
	delete linkptr;
	return 0;
}

static int linksFileThread(void*) {
	LinksFileDevice dev("/sys/net/links",0444);
	dev.loop();
	return 0;
}

static int routesFileThread(void*) {
	RoutesFileDevice dev("/sys/net/routes",0444);
	dev.loop();
	return 0;
}

static int arpFileThread(void*) {
	ARPFileDevice dev("/sys/net/arp",0444);
	dev.loop();
	return 0;
}

static int socketsFileThread(void*) {
	SocketsFileDevice dev("/sys/net/sockets",0444);
	dev.loop();
	return 0;
}

static int socketThread(void*) {
	SocketDevice dev("/dev/socket",0777);
	dev.loop();
	return 0;
}

static void createResolvConf() {
	// create file
	const char *resolvconf = ask::DNS::getResolveFile();
	print("Creating empty %s",resolvconf);
	int fd = create(resolvconf,O_WRONLY | O_CREAT,0660);
	if(fd < 0) {
		printe("Unable to create %s",resolvconf);
		return;
	}
	close(fd);

	// chown it to the network-group
	size_t groupCount;
	sGroup *groups = group_parseFromFile(GROUPS_PATH,&groupCount);
	sGroup *network = group_getByName(groups,"network");
	if(!network || chown(resolvconf,0,network->gid) < 0)
		printe("Unable to chmod %s",resolvconf);
	group_free(groups);
}

int main() {
	srand(rdtsc());

	print("Creating /sys/net");
	if(mkdir("/sys/net",DIR_DEF_MODE) < 0)
		printe("Unable to create /sys/net");
	createResolvConf();

	if(startthread(socketThread,NULL) < 0)
		error("Unable to start socket thread");
	if(startthread(linksFileThread,NULL) < 0)
		error("Unable to start links-file thread");
	if(startthread(routesFileThread,NULL) < 0)
		error("Unable to start routes-file thread");
	if(startthread(arpFileThread,NULL) < 0)
		error("Unable to start arp-file thread");
	if(startthread(socketsFileThread,NULL) < 0)
		error("Unable to start sockets-file thread");
	if(startthread(Timeouts::thread,NULL) < 0)
		error("Unable to start timeout thread");

	NetDevice netdev("/dev/tcpip",0111);
	netdev.loop();
	return 0;
}
