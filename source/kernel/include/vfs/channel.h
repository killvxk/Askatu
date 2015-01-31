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

#include <ask/col/slist.h>
#include <vfs/node.h>
#include <common.h>

class VFSChannel : public VFSNode {
	struct Message : public ask::SListItem {
		msgid_t id;
		size_t length;
	};

public:
	/**
	 * Creates a new channel for given process
	 *
	 * @param pid the process-id
	 * @param parent the parent-node
	 * @param success whether the constructor succeeded (is expected to be true before the call!)
	 */
	explicit VFSChannel(pid_t pid,VFSNode *parent,bool &success);

	/**
	 * @return the file-descriptor for the driver to communicate with this channel
	 */
	int getFd() const {
		return fd;
	}

	/**
	 * @return the thread who handles this channel
	 */
	tid_t getHandler() const {
		return handler;
	}
	/**
	 * Binds this channel to the given thread, i.e. changes the handler
	 *
	 * @param tid the thread-id
	 */
	void bindto(tid_t tid) {
		handler = tid;
	}

	/**
	 * Checks whether the channel has work to do for the server
	 *
	 * @return true if so
	 */
	bool hasWork() const {
		return sendList.length() > 0;
	}

	/**
	 * Removes all messages from the send- and receive-list.
	 */
	void discardMsgs();

	/**
	 * Sends the given message to the channel
	 *
	 * @param pid the process-id
	 * @param flags the flags of the file
	 * @param id the message-id
	 * @param data1 the message-data
	 * @param size1 the data-size
	 * @param data2 for the device-messages: a second message (NULL = no second one)
	 * @param size2 the size of the second message
	 * @return 0 on success
	 */
	ssize_t send(pid_t pid,ushort flags,msgid_t id,USER const void *data1,size_t size1,
	             USER const void *data2,size_t size2);

	/**
	 * Receives a message from the channel
	 *
	 * @param pid the process-id
	 * @param flags the flags of the file
	 * @param id will be set to the message-id (if not NULL)
	 * @param data the buffer to write the message to
	 * @param size the size of the buffer
	 * @return the number of written bytes on success
	 */
	ssize_t receive(pid_t pid,ushort flags,msgid_t *id,void *data,size_t size);

	/**
	 * Cancels the message <mid> that is currently in flight. If the device supports it, it waits
	 * until it has received the response. This tells us whether the message has been canceled or if
	 * the response has already been sent.
	 *
	 * @param pid the process-id
	 * @param file the file
	 * @param mid the message-id to cancel
	 * @return 0 if it has been canceled, 1 if the reply is already available or < 0 on errors
	 */
	int cancel(pid_t pid,OpenFile *file,msgid_t mid);

	/**
	 * Shares a file with the device that hosts this channel.
	 *
	 * @param pid the process-id
	 * @param file the file
	 * @param path the path to the file
	 * @param cliaddr the address where the client of this channel has mapped it
	 * @param size the size of the mmap'd file
	 * @return 0 on success
	 */
	int sharefile(pid_t pid,OpenFile *file,const char *path,void *cliaddr,size_t size);

	/**
	 * Creates a new sibling-channel for <file>.
	 *
	 * @param pid the process-id
	 * @param file the current channel
	 * @param sibl the sibling channel
	 * @param arg an arbitrary argument to send to the driver
	 * @return 0 on success
	 */
	int creatsibl(pid_t pid,OpenFile *file,VFSChannel *sibl,int arg);

	/**
	 * Sends the stat-message to this channel. This is only used for fs instances.
	 *
	 * @param pid the process-id
	 * @param info the file info
	 * @return 0 on success
	 */
	int stat(pid_t pid,USER struct stat *info);

	virtual ssize_t open(pid_t pid,const char *path,uint flags,int msgid,mode_t mode);
	virtual off_t seek(pid_t pid,off_t position,off_t offset,uint whence) const;
	virtual ssize_t getSize(pid_t pid);
	virtual ssize_t read(pid_t pid,OpenFile *file,void *buffer,off_t offset,size_t count);
	virtual ssize_t write(pid_t pid,OpenFile *file,const void *buffer,off_t offset,size_t count);
	virtual void close(pid_t pid,OpenFile *file,int msgid);
	virtual void print(OStream &os) const;

protected:
	virtual void invalidate();

private:
	static Message *getMsg(ask::SList<Message> *list,msgid_t mid,ushort flags);
	uint getReceiveFlags() const;
	int isSupported(int op) const;
	int openForDriver();
	void closeForDriver();

	int fd;
	tid_t handler;
	bool closed;
	void *shmem;
	size_t shmemSize;
	/* a list for sending messages to the device */
	ask::SList<Message> sendList;
	/* a list for reading messages from the device */
	ask::SList<Message> recvList;
	static uint16_t nextRid;
};
