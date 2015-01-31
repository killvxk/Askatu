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

#include "ext2.h"

class Ext2Link {
	Ext2Link() = delete;

public:
	/**
	 * Creates a entry for cnode->inodeNo+name in the given directory. Increases the link-count
	 * for the given inode.
	 *
	 * @param e the ext2-data
	 * @param u the user
	 * @param dir the directory (requested for writing!)
	 * @param cnode the cached inode
	 * @param name the name
	 * @return 0 on success
	 */
	static int create(Ext2FileSystem *e,FSUser *u,Ext2CInode *dir,Ext2CInode *cnode,const char *name);

	/**
	 * Removes the given name from the given directory
	 *
	 * @param e the ext2-data
	 * @param u the user
	 * @param pdir if available, the parent-directory (requested for writing!)
	 * @param dir the directory (requested for writing!)
	 * @param name the entry-name
	 * @param delDir whether the entry may be an directory
	 * @return 0 on success
	 */
	static int remove(Ext2FileSystem *e,FSUser *u,Ext2CInode *pdir,Ext2CInode *dir,const char *name,
		bool delDir);

private:
	/**
	 * Calculates the total size of a dir-entry, including padding
	 */
	static size_t getDirESize(size_t namelen);
};
