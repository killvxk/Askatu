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

/* first sector of the super-block in the first block-group */
#define EXT2_SUPERBLOCK_SECNO				2
/* first block-number of the block-group-daskriptor-table in other block-groups than the first */
#define EXT2_BLOGRPTBL_BNO					2

/* number of direct blocks in the inode */
#define EXT2_DIRBLOCK_COUNT					12

/* padding for directory-entries */
#define	EXT2_DIRENTRY_PAD					4

/* magic number */
#define EXT2_SUPER_MAGIC					0xEF53

/* states */
#define EXT2_VALID_FS						1
#define EXT2_ERROR_FS						2

/* how to handle errors */
/* continue as if nothing happened */
#define EXT2_ERRORS_CONTINUE				1
/* remount read-only */
#define EXT2_ERRORS_RO						2
/* cause a kernel panic */
#define EXT2_ERRORS_PANIC					3

/* creator os's */
#define EXT2_OS_LINUX						0
#define EXT2_OS_HURD						1
#define EXT2_OS_MASIX						2
#define EXT2_OS_FREEBSD						3
#define EXT2_OS_LITES						4

/* revisions */
#define EXT2_GOOD_OLD_REV					0
/*  Revision 1 with variable inode sizes, extended attributes, etc. */
#define EXT2_DYNAMIC_REV					1

/* first inode-number for standard-files in revision 0 */
#define EXT2_REV0_FIRST_INO					11
/* inode-size in rev0 */
#define EXT2_REV0_INODE_SIZE				128

/* compatible features */
#define EXT2_FEATURE_COMPAT_DIR_PREALLOC	0x0001
#define EXT2_FEATURE_COMPAT_IMAGIC_INODES	0x0002
#define EXT3_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT2_FEATURE_COMPAT_EXT_ATTR		0x0008
#define EXT2_FEATURE_COMPAT_RESIZE_INO		0x0010
#define EXT2_FEATURE_COMPAT_DIR_INDEX		0x0020

/* incompatible features */
#define EXT2_FEATURE_INCOMPAT_COMPRESSION	0x0001
#define EXT2_FEATURE_INCOMPAT_FILETYPE		0x0002
#define EXT3_FEATURE_INCOMPAT_RECOVER		0x0004
#define EXT3_FEATURE_INCOMPAT_JOURNAL_DEV	0x0008
#define EXT2_FEATURE_INCOMPAT_META_BG		0x0010

/* read-only features */
#define EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER	0x0001
#define EXT2_FEATURE_RO_COMPAT_LARGE_FILE	0x0002
#define EXT2_FEATURE_RO_COMPAT_BTREE_DIR	0x0004

/* compression algorithms */
#define EXT2_LZV1_ALG						0x0001
#define EXT2_LZRW3A_ALG						0x0002
#define EXT2_GZIP_ALG						0x0004
#define EXT2_BZIP2_ALG						0x0008
#define EXT2_LZO_ALG						0x0010

/* reserved inode-numbers */
#define EXT2_BAD_INO						1
#define EXT2_ROOT_INO						2
#define EXT2_ACL_IDX_INO					3
#define EXT2_ACL_DATA_INO					4
#define EXT2_BOOT_LOADER_INO				5
#define EXT2_UNDEL_DIR_INO					6

/* mode flags */
/* file format */
#define EXT2_S_IFSOCK						0xC000
#define EXT2_S_IFLNK						0xA000
#define EXT2_S_IFREG						0x8000
#define EXT2_S_IFBLK						0x6000
#define EXT2_S_IFDIR						0x4000
#define EXT2_S_IFCHR						0x2000
#define EXT2_S_IFIFO						0x1000
/* process execution user/group override */
#define EXT2_S_ISUID						0x0800	/* Set process User ID */
#define EXT2_S_ISGID						0x0400	/* Set process Group ID */
#define EXT2_S_ISVTX						0x0200	/* sticky bit */
/* access rights */
#define EXT2_S_IRUSR						0x0100
#define EXT2_S_IWUSR						0x0080
#define EXT2_S_IXUSR						0x0040
#define EXT2_S_IRGRP						0x0020
#define EXT2_S_IWGRP						0x0010
#define EXT2_S_IXGRP						0x0008
#define EXT2_S_IROTH						0x0004
#define EXT2_S_IWOTH						0x0002
#define EXT2_S_IXOTH						0x0001

#define EXT2_S_PERMS						0x0FFF

/* flags */
#define EXT2_SECRM_FL						0x00000001	/* secure deletion */
#define EXT2_UNRM_FL						0x00000002	/* record for undelete */
#define EXT2_COMPR_FL						0x00000004	/* compressed file */
#define EXT2_SYNC_FL						0x00000008	/* synchronous updates */
#define EXT2_IMMUTABLE_FL					0x00000010	/* immutable file */
#define EXT2_APPEND_FL						0x00000020	/* append only */
#define EXT2_NODUMP_FL						0x00000040	/* do not dump/delete file */
#define EXT2_NOATIME_FL						0x00000080	/* do not update .accesstime */
/* for compression: */
#define EXT2_DIRTY_FL						0x00000100
#define EXT2_COMPRBLK_FL					0x00000200	/* compressed blocks */
#define EXT2_NOCOMPR_FL						0x00000400	/* access raw compressed data */
#define EXT2_ECOMPR_FL						0x00000800	/* compression error */
/* compression end */
#define EXT2_BTREE_FL						0x00010000	/* b-tree format directory */
#define EXT2_INDEX_FL						0x00010000  /* hash indexed directory */
#define EXT2_IMAGIC_FL						0x00020000	/* AFS directory */
#define EXT3_JOURNAL_DATA_FL				0x00040000	/* journal file data */
#define EXT2_RESERVED_FL					0x80000000	/* reserved for ext2 library */

struct Ext2SuperBlock {
	/* the total number of inodes, both used and free, in the file system. */
	uint32_t inodeCount;
	/* the total number of blocks in the system including all used, free and reserved */
	uint32_t blockCount;
	/* number of blocks reserved for the super-user */
	uint32_t suResBlockCount;
	uint32_t freeBlockCount;
	uint32_t freeInodeCount;
	uint32_t firstDataBlock;
	/* blockSize = 1024 << logBlockSize; */
	uint32_t logBlockSize;
	/* if(positive)
	 *   fragmentSize = 1024 << logFragSize;
	 * else
	 *   fragmentSize = 1024 >> -logFragSize; */
	uint32_t logFragSize;
	uint32_t blocksPerGroup;
	uint32_t fragsPerGroup;
	uint32_t inodesPerGroup;
	uint32_t lastMountTime;
	uint32_t lastWritetime;
	/* number of mounts since last verification */
	uint16_t mountCount;
	/* max number of mounts until a full check has to be performed */
	uint16_t maxMountCount;
	/* should be EXT2_SUPER_MAGIC */
	uint16_t magic;
	/* EXT2_VALID_FS or EXT2_ERROR_FS */
	uint16_t state;
	/* on of EXT2_ERRORS_* */
	uint16_t errors;
	uint16_t minorRevLevel;
	uint32_t lastCheck;
	uint32_t checkInterval;
	uint32_t creatorOS;
	uint32_t revLevel;
	/* the default user-/group-id for reserved blocks */
	uint16_t defResUid;
	uint16_t defResGid;
	/* the index to the first inode useable for standard files. Always EXT2_REV0_FIRST_INO for
	 * revision 0. In revision 1 and later this value may be set to any value. */
	uint32_t firstInode;
	/* 16bit value indicating the size of the inode structure. In revision 0, this value is always
	 * 128 (EXT2_REV0_INODE_SIZE). In revision 1 and later, this value must be a perfect power of
	 * 2 and must be smaller or equal to the block size (1<<s_log_block_size). */
	uint16_t inodeSize;
	/* the block-group which hosts this super-block */
	uint16_t blockGroupNo;
	/* 32bit bitmask of compatible features */
	uint32_t featureCompat;
	/* 32bit bitmask of incompatible features */
	uint32_t featureInCompat;
	/* 32bit bitmask of “read-only” features. The file system implementation should mount as
	 * read-only if any of the indicated feature is unsupported. */
	uint32_t featureRoCompat;
	/* 128bit value used as the volume id. This should, as much as possible, be unique for
	 * each file system formatted. */
	uint8_t volumeUid[16];
	/* 16 bytes volume name, mostly unusued. A valid volume name would consist of only ISO-Latin-1
	 * characters and be 0 terminated. */
	char volumeName[16];
	/* 64 bytes directory path where the file system was last mounted. */
	char lastMountPath[64];
	/* 32bit value used by compression algorithms to determine the compression method(s) used. */
	uint32_t algoBitmap;
	/* 8-bit value representing the number of blocks the implementation should attempt to
	 * pre-allocate when creating a new regular file. */
	uint8_t preAllocBlocks;
	/* 8-bit value representing the number of blocks the implementation should attempt to
	 * pre-allocate when creating a new directory. */
	uint8_t preAllocDirBlocks;
	/* alignment */
	uint16_t : 16;
	/* 16-byte value containing the uuid of the journal superblock */
	uint8_t journalUid[16];
	/* 32-bit inode number of the journal file */
	uint32_t journalInodeNo;
	/* 32-bit device number of the journal file */
	uint32_t journalDev;
	/* 32-bit inode number, pointing to the first inode in the list of inodes to delete. */
	uint32_t lastOrphan;
	/* An array of 4 32bit values containing the seeds used for the hash algorithm for directory
	 * indexing. */
	uint32_t hashSeed[4];
	/* An 8bit value containing the default hash version used for directory indexing.*/
	uint8_t defHashVersion;
	/* padding */
	uint16_t : 16;
	uint8_t : 8;
	/* A 32bit value containing the default mount options for this file system. */
	uint32_t defMountOptions;
	/* A 32bit value indicating the block group ID of the first meta block group. */
	uint32_t firstMetaBg;
	/* UNUSED */
	uint8_t unused[760];
} A_PACKED;

struct Ext2BlockGrp {
	/* block id of the first block of the "block bitmap" for the group represented. */
	uint32_t blockBitmap;
	/* block id of the first block of the "inode bitmap" for the group represented. */
	uint32_t inodeBitmap;
	/* block id of the first block of the "inode table" for the group represented. */
	uint32_t inodeTable;
	uint16_t freeBlockCount;
	uint16_t freeInodeCount;
	/* the number of inodes allocated to directories for the represented group. */
	uint16_t usedDirCount;
	/* padding */
	uint16_t : 16;
	/* reserved */
	uint32_t : 32;
	uint32_t : 32;
	uint32_t : 32;
} A_PACKED;

struct Ext2DirEntry {
	ino_t inode;
	uint16_t recLen;
	uint16_t nameLen;
	/* name follows (up to 255 bytes) */
	char name[];
} A_PACKED;

struct Ext2Inode {
	uint16_t mode;
	uint16_t uid;
	int32_t size;
	uint32_t accesstime;
	uint32_t createtime;
	uint32_t modifytime;
	uint32_t deletetime;
	uint16_t gid;
	/* number of links to this inode (when it reaches 0 the inode and all its associated blocks
	 * are freed) */
	uint16_t linkCount;
	/* The total number of blocks reserved to contain the data of this inode. That means including
	 * the blocks with block-numbers. It is not measured in ext2-blocks, but in sectors! */
	uint32_t blocks;
	uint32_t flags;
	/* OS dependant value. */
	uint32_t osd1;
	/* A value of 0 in the block-array effectively terminates it with no further block being defined.
	 * All the remaining entries of the array should still be set to 0. */
	uint32_t dBlocks[12];
	uint32_t singlyIBlock;
	uint32_t doublyIBlock;
	uint32_t triplyIBlock;
	/* used to indicate the file version (used by NFS). */
	uint32_t generation;
	/* indicating the block number containing the extended attributes. In revision 0 this value
	 * is always 0. */
	uint32_t fileACL;
	/* In revision 0 this 32bit value is always 0. In revision 1, for regular files this 32bit
	 * value contains the high 32 bits of the 64bit file size. */
	uint32_t dirACL;
	/* 32bit value indicating the location of the file fragment. */
	uint32_t fragAddr;
	/* 96bit OS dependant structure. */
	uint16_t osd2[6];
} A_PACKED;
