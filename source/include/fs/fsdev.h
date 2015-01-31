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

#include <ask/ipc/clientdevice.h>
#include <fs/common.h>
#include <fs/infodev.h>
#include <sys/common.h>
#include <stdio.h>

class FileSystem;

struct OpenFile : public ask::Client {
public:
	explicit OpenFile(int fd) : Client(fd), ino() {
	}
	explicit OpenFile(int fd,ino_t _ino) : Client(fd), ino(_ino) {
	}

	ino_t ino;
};

class FSDevice : public ask::ClientDevice<OpenFile> {
public:
	static FSDevice *getInstance() {
		return _inst;
	}

	explicit FSDevice(FileSystem *fs,const char *diskDev);
	virtual ~FSDevice();

	void loop();

	void devopen(ask::IPCStream &is);
	void devclose(ask::IPCStream &is);
	void open(ask::IPCStream &is);
	void read(ask::IPCStream &is);
	void write(ask::IPCStream &is);
	void close(ask::IPCStream &is);
	void stat(ask::IPCStream &is);
	void istat(ask::IPCStream &is);
	void syncfs(ask::IPCStream &is);
	void link(ask::IPCStream &is);
	void unlink(ask::IPCStream &is);
	void rename(ask::IPCStream &is);
	void mkdir(ask::IPCStream &is);
	void rmdir(ask::IPCStream &is);
	void chmod(ask::IPCStream &is);
	void chown(ask::IPCStream &is);
	void utime(ask::IPCStream &is);

private:
	const char *resolveDir(FSUser *u,char *path,ino_t *ino);

	FileSystem *_fs;
	InfoDevice _info;
	size_t _clients;
	static FSDevice *_inst;
};
