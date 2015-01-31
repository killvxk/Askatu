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
#include <sys/syscalls.h>
#include <errno.h>
#include <stdarg.h>

/* access mode */
#define O_READ					1
#define O_WRITE					2
#define O_MSGS					4					/* exchange messages with a device */
#define O_RDONLY				O_READ
#define O_WRONLY				O_WRITE
#define O_RDWR					(O_READ | O_WRITE)
#define O_RDWRMSG				(O_READ | O_WRITE | O_MSGS)
#define O_ACCMODE	   			O_RDWRMSG

/* open flags */
#define O_CREAT					8
#define O_TRUNC					16
#define O_APPEND				32
#define O_NONBLOCK				64	/* don't block when reading or receiving a msg from devices */
#define O_LONELY				128	/* disallow other accesses */
#define O_EXCL					256	/* fail if the file already exists */

/* file daskriptors for stdin, stdout and stderr */
#define STDIN_FILENO			0
#define STDOUT_FILENO			1
#define STDERR_FILENO			2

/* fcntl-commands */
#define F_GETFL					0
#define F_SETFL					1
#define F_GETACCESS				2
#define F_SEMUP					3
#define F_SEMDOWN				4
#define F_DISMSGS				5

/* seek-types */
#define SEEK_SET				0
#define SEEK_CUR				1
#define SEEK_END				2

/* retry a syscall until it succeeded, skipping tries that failed because of a signal */
#define IGNSIGS(expr) ({ \
		int __err; \
		do { \
			__err = (expr); \
		} \
		while(__err == -EINTR); \
		__err; \
	})

/* do a sendrecv() and skip tries that failed because of a signal */
#define SENDRECV_IGNSIGS(fd,mid,msg,size) ({ \
	int __err = sendrecv(fd,mid,msg,size); \
	while(__err == -EINTR) { \
		__err = receive(fd,mid,msg,size); \
	} \
	__err; \
})

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Opens the given path with given mode and returns the associated file-daskriptor
 *
 * @param path the path to open
 * @param flags the open flags (IO_*)
 * @return the file-daskriptor; negative if error
 */
A_CHECKRET int open(const char *path,uint flags);

/**
 * Creates the given path with given flags and mode and returns the associated file-daskriptor
 *
 * @param path the path to open
 * @param flags the open flags (IO_*)
 * @param mode the mode for the created file
 * @return the file-daskriptor; negative if error
 */
A_CHECKRET int create(const char *path,uint flags,mode_t mode);

/**
 * Creates a pipe with 2 separate files for reading and writing.
 *
 * @param readFd will be set to the fd for reading
 * @param writeFd will be set to the fd for writing
 * @return 0 on success
 */
A_CHECKRET int pipe(int *readFd,int *writeFd);

/**
 * Asks for the current file-position
 *
 * @param fd the file-daskriptor
 * @param pos will point to the current file-position on success
 * @return 0 on success
 */
A_CHECKRET static inline int tell(int fd,off_t *pos) {
	return syscall2(SYSCALL_TELL,fd,(ulong)pos);
}

/**
 * Manipulates the given file-daskriptor, depending on the command
 *
 * @param fd the file-daskriptor
 * @param cmd the command (F_GETFL or F_SETFL)
 * @param arg the argument (just used for F_SETFL)
 * @return >= 0 on success
 */
static inline int fcntl(int fd,uint cmd,int arg) {
	return syscall3(SYSCALL_FCNTL,fd,cmd,arg);
}

/**
 * Changes the position in the given file
 *
 * @param fd the file-daskriptor
 * @param offset the offset
 * @param whence the seek-type: SEEK_SET, SEEK_CUR or SEEK_END
 * @return the new position on success, of the negative error-code
 */
A_CHECKRET static inline off_t seek(int fd,off_t offset,uint whence) {
	return syscall3(SYSCALL_SEEK,fd,offset,whence);
}

/**
 * Reads count bytes from the given file-daskriptor into the given buffer and returns the
 * actual read number of bytes. You may be interrupted by a signal (-EINTR)!
 *
 * @param fd the file-daskriptor
 * @param buffer the buffer to fill
 * @param count the number of bytes
 * @return the actual read number of bytes; negative if an error occurred
 */
A_CHECKRET static inline ssize_t read(int fd,void *buffer,size_t count) {
	return syscall3(SYSCALL_READ,fd,(ulong)buffer,count);
}

/**
 * Writes count bytes from the given buffer into the given fd and returns the number of written
 * bytes.
 *
 * @param fd the file-daskriptor
 * @param buffer the buffer to read from
 * @param count the number of bytes to write
 * @return the number of bytes written; negative if an error occurred
 */
A_CHECKRET static inline ssize_t write(int fd,const void *buffer,size_t count) {
	return syscall3(SYSCALL_WRITE,fd,(ulong)buffer,count);
}

/**
 * Sends a message to the device identified by <fd>.
 *
 * @param fd the file-daskriptor
 * @param id the msg-id
 * @param msg the message (may be NULL)
 * @param size the size of the message
 * @return the used message-id on success or < 0 if an error occurred
 */
static inline ssize_t send(int fd,msgid_t id,const void *msg,size_t size) {
	return syscall4(SYSCALL_SEND,fd,id,(ulong)msg,size);
}

/**
 * Receives the message from the device identified by <fd> with id *<id> or the next "for anybody"
 * message if <id> is NULL or *<id> is 0. Blocks if that message is not available.
 * You may be interrupted by a signal (-EINTR)!
 *
 * @param fd the file-daskriptor
 * @param id will be set to the msg-id (may be NULL)
 * @param msg the message (may be NULL to skip the message)
 * @param size the (max) size of the message
 * @return the size of the message
 */
A_CHECKRET static inline ssize_t receive(int fd,msgid_t *id,void *msg,size_t size) {
	return syscall4(SYSCALL_RECEIVE,fd,(ulong)id,(ulong)msg,size);
}

/**
 * Sends a message to the device identified by <fd> and receives the response.
 * Note that if you get interrupted by a signal, you'll receive -EINTR, but the message will
 * have been already sent in all cases, because it can only happen during the receive.
 *
 * @param fd the file-daskriptor
 * @param id the msg-id; will hold the received message-id afterwards
 * @param msg the message (may be NULL)
 * @param size the size of the message
 * @return 0 on success or < 0 if an error occurred
 */
A_CHECKRET static inline ssize_t sendrecv(int fd,msgid_t *id,void *msg,size_t size) {
	return syscall4(SYSCALL_SENDRECV,fd,(ulong)id,(ulong)msg,size);
}

/**
 * Cancels the message <mid> that is currently in flight on the channel denoted by <fd>. If the
 * device supports it, it waits until it has received the response. This tells us whether the
 * message has been canceled or if the response has already been sent.
 *
 * @param fd the file-daskriptor for the channel
 * @param mid the message-id to cancel
 * @return 0 if it has been canceled, 1 if the reply is already available or < 0 on errors
 */
A_CHECKRET static inline int cancel(int fd,msgid_t mid) {
	return syscall2(SYSCALL_CANCEL,fd,mid);
}

/**
 * Creates a new sibling channel for the channel denoted by <fd>. The channel is not special in any
 * sense, but only created with the driver knowing the relation to the channel <fd> instead of
 * opening a new one. Thus, it can be used to associate channels with each other.
 *
 * @param fd the file daskriptor for an existing channel
 * @param arg an arbitrary argument that is passed to the driver
 * @return the file daskriptor for the created sibling channel (or a negative error-code)
 */
A_CHECKRET static inline int creatsibl(int fd,int arg) {
	return syscall2(SYSCALL_CREATSIBL,fd,arg);
}

/**
 * Shares the file, denoted by <mem>, with the device, denoted by <dev>. That is, it sends
 * a message to the device and asks him to join that memory, if he wants to. The parameter <mem>
 * has to point to the beginning of the area where a file has been mmap'd.
 * If a read or write is performed using <mem> as buffer, the data will be exchanged over this
 * shared memory instead of copying it twice.
 *
 * @param dev the file daskriptor to the device
 * @param fd the file daskriptor to the shared-memory-file
 * @param mem the memory area where <fd> has been mmap'd
 * @return 0 on success
 */
A_CHECKRET static inline int sharefile(int dev,void *mem) {
	return syscall2(SYSCALL_SHAREFILE,dev,(ulong)mem);
}

/**
 * Uses pshm_create() to create a shared memory file, mmap's it with size <size> and shares it with
 * the device denoted by <dev>. If everything worked except for the sharing, *mem will be non-zero
 * but the function returns a negative error-code.
 *
 * @param dev the file daskriptor to the device
 * @param size the size of the buffer
 * @param mem will be set to the buffer address (!= NULL means only the sharing failed)
 * @param name will be set to the shared memory name
 * @param flags the flags to pass to mmap (besides MAP_SHARED)
 * @return 0 on success
 */
int sharebuf(int dev,size_t size,void **mem,ulong *name,int flags);

/**
 * Joins the given shared memory file, i.e. opens <path> and mmaps it.
 *
 * @param path the sh-mem file
 * @param size the size to pass to mmap
 * @param flags the flags to pass to mmap (besides MAP_SHARED)
 * @return the address or NULL
 */
void *joinbuf(const char *path,size_t size,int flags);

/**
 * Destroys the shared memory file, that has been created by sharebuf. That is, it unlinks the file
 * and munmaps the given memory region
 *
 * @param mem the memory region
 * @param name the name of the shared memory file
 */
void destroybuf(void *mem,ulong name);

/**
 * Duplicates the given file-daskriptor
 *
 * @param fd the file-daskriptor
 * @return the error-code or the new file-daskriptor
 */
static inline int dup(int fd) {
	return syscall1(SYSCALL_DUPFD,fd);
}

/**
 * Redirects <src> to <dst>. <src> will be closed. Note that both fds have to exist!
 *
 * @param src the source-file-daskriptor
 * @param dst the destination-file-daskriptor
 * @return the error-code or 0 if successfull
 */
static inline int redirect(int src,int dst) {
	return syscall2(SYSCALL_REDIRFD,src,dst);
}

/**
 * Creates a hardlink at <newPath> which points to <oldPath>
 *
 * @param oldPath the link-target
 * @param newPath the link-path
 * @return 0 on success
 */
A_CHECKRET int link(const char *oldPath,const char *newPath);

/**
 * Unlinks the given path. That means, the directory-entry will be removed and if there are no
 * more references to the inode, it will be removed.
 *
 * @param path the path
 * @return 0 on success
 */
A_CHECKRET int unlink(const char *path);

/**
 * Renames <oldPath> to <newPath>. Note that you shouldn't assume that link+unlink works for every
 * filesystem, but use rename instead if you want to rename something. E.g. ftpfs does not have
 * support for link.
 *
 * @param oldPath the path to rename
 * @param newPath the new path
 * @return 0 on success
 */
A_CHECKRET int rename(const char *oldPath,const char *newPath);

/**
 * Creates the given directory. Expects that all except the last path-component exist.
 *
 * @param path the path
 * @param mode the mode for the created file
 * @return 0 on success
 */
A_CHECKRET int mkdir(const char *path,mode_t mode);

/**
 * Removes the given directory. Expects that the directory is empty (except '.' and '..')
 *
 * @param path the path
 * @return 0 on success
 */
A_CHECKRET int rmdir(const char *path);

/**
 * Writes all dirty objects of the affected filesystem to disk
 *
 * @param fd the file-daskriptor to some file on that fs
 * @return 0 on success
 */
A_CHECKRET static inline int syncfs(int fd) {
	return syscall1(SYSCALL_SYNCFS,fd);
}

/**
 * Closes the given file-daskriptor
 *
 * @param fd the file-daskriptor
 */
static inline void close(int fd) {
	syscall1(SYSCALL_CLOSE,fd);
}

/**
 * Performs the semaphore up-operation on the given file.
 *
 * @param fd the file-daskriptor
 * @return 0 on success
 */
static inline int fsemup(int fd) {
	return fcntl(fd,F_SEMUP,0);
}

/**
 * Performs the semaphore down-operation on the given file.
 *
 * @param fd the file-daskriptor
 * @return 0 on success
 */
static inline int fsemdown(int fd) {
	return fcntl(fd,F_SEMDOWN,0);
}

#if defined(__cplusplus)
}
#endif
