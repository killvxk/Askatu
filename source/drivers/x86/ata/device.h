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
#include <sys/irq.h>

#include "partition.h"

#define OP_READ						0
#define OP_WRITE					1
#define OP_PACKET					2

#define DMA_TRANSFER_TIMEOUT		3000
#define DMA_TRANSFER_SLEEPTIME		20

/* sleep for 20ms (just for writes; when reading we wait for an interrupt; the status should be ok
 * afterwards) */
#define PIO_TRANSFER_TIMEOUT		3000
#define PIO_TRANSFER_SLEEPTIME		0

#define ATAPI_TRANSFER_TIMEOUT		3000
#define ATAPI_TRANSFER_SLEEPTIME	20

#define ATAPI_WAIT_TIMEOUT			5000
#define ATA_WAIT_TIMEOUT			500
#define ATA_WAIT_SLEEPTIME			20

#define IRQ_POLL_INTERVAL			20		/* in ms */
#define IRQ_TIMEOUT					5000	/* in ms */

/* port-bases */
#define ATA_REG_BASE_PRIMARY		0x1F0
#define ATA_REG_BASE_SECONDARY		0x170

#define DRIVE_MASTER				0xA0
#define DRIVE_SLAVE					0xB0

#define SLAVE_BIT					0x1

#define COMMAND_IDENTIFY			0xEC
#define COMMAND_IDENTIFY_PACKET		0xA1
#define COMMAND_READ_SEC			0x20
#define COMMAND_READ_SEC_EXT		0x24
#define COMMAND_WRITE_SEC			0x30
#define COMMAND_WRITE_SEC_EXT		0x34
#define COMMAND_READ_DMA			0xC8
#define COMMAND_READ_DMA_EXT		0x25
#define COMMAND_WRITE_DMA			0xCA
#define COMMAND_WRITE_DMA_EXT		0x35
#define COMMAND_PACKET				0xA0
#define COMMAND_ATAPI_RESET			0x8

#define SCSI_CMD_READ_SECTORS		0x28
#define SCSI_CMD_READ_SECTORS_EXT	0xA8
#define SCSI_CMD_READ_CAPACITY		0x25

/* io-ports, offsets from base */
#define ATA_REG_DATA				0x0
#define ATA_REG_ERROR				0x1
#define ATA_REG_FEATURES			0x1
#define ATA_REG_SECTOR_COUNT		0x2
#define ATA_REG_ADDRESS1			0x3
#define ATA_REG_ADDRESS2			0x4
#define ATA_REG_ADDRESS3			0x5
#define ATA_REG_DRIVE_SELECT		0x6
#define ATA_REG_COMMAND				0x7
#define ATA_REG_STATUS				0x7
#define ATA_REG_CONTROL				0x206

#define ATAPI_SEC_SIZE				2048
#define ATA_SEC_SIZE				512

/* the LBA-flag for the device-register */
#define DEVICE_LBA					0x40

/* Drive is preparing to accept/send data -- wait until this bit clears. If it never
 * clears, do a Software Reset. Technically, when BSY is set, the other bits in the
 * Status byte are meaningless. */
#define CMD_ST_BUSY					(1 << 7)	/* 0x80 */
/* Bit is clear when device is spun down, or after an error. Set otherwise. */
#define CMD_ST_READY				(1 << 6)	/* 0x40 */
/* Drive Fault Error (does not set ERR!) */
#define CMD_ST_DISK_FAULT			(1 << 5)	/* 0x20 */
/* Overlapped Mode Service Request */
#define CMD_ST_OVERLAPPED_REQ		(1 << 4)	/* 0x10 */
/* Set when the device has PIO data to transfer, or is ready to accept PIO data. */
#define CMD_ST_DRQ					(1 << 3)	/* 0x08 */
/* Error flag (when set). Send a new command to clear it (or nuke it with a Software Reset). */
#define CMD_ST_ERROR				(1 << 0)	/* 0x01 */

/* Set this to read back the High Order Byte of the last LBA48 value sent to an IO port. */
#define CTRL_HIGH_ORDER_BYTE		(1 << 7)	/* 0x80 */
/* Software Reset -- set this to reset all ATA drives on a bus, if one is misbehaving. */
#define CTRL_SOFTWARE_RESET			(1 << 2)	/* 0x04 */
/* Set this to stop the current device from sending interrupts. */
#define CTRL_NIEN					(1 << 1)	/* 0x02 */

typedef struct {
	struct {
		/* reserved / obsolete / retired / ... */
		uint16_t : 7,
		remMediaDevice : 1,
		/* retired */
		: 7,
		/* 0 = ATA, 1 = ATAPI */
		isATAPI : 1;
	} A_PACKED general;
	uint16_t oldCylinderCount;
	/* specific configuration */
	uint16_t : 16;
	uint16_t oldHeadCount;
	uint16_t oldUnformBytesPerTrack;
	uint16_t oldUnformBytesPerSec;
	uint16_t oldSecsPerTrack;
	/* reserved for assignment by the compactflash association */
	uint16_t : 16;
	uint16_t : 16;
	/* retired */
	uint16_t : 16;
	/* 20 ASCII chars */
	char serialNumber[20];
	/* retired */
	uint16_t : 16;
	uint16_t : 16;
	/* obsolete */
	uint16_t : 16;
	/* 8 ASCII chars, 0000h = not specified */
	char firmwareRev[8];
	/* 40 ASCII chars, 0000h = not specified */
	char modelNo[40];
	/* 00h = read/write multiple commands not implemented.
	 * xxh = Maximum number of sectors that can be transferred per interrupt on read and write
	 * 	multiple commands */
	uint8_t maxSecsPerIntrpt;
	/* always 0x80 */
	uint8_t : 8;
	/* reserved */
	uint16_t : 16;
	struct {
		/* retired */
		uint16_t : 8,
		DMA : 1,
		LBA : 1,
		/* IORDY may be disabled */
		IORDYDisabled : 1,
		/* 0 = IORDY may be supported */
		IORDYSupported : 1,
		/* reserved / uninteresting */
		: 4;
	} A_PACKED capabilities;
	/* further capabilities */
	uint16_t : 16;
	/* obsolete */
	uint16_t : 16;
	uint16_t : 16;
	uint16_t words5458Valid : 1,
	words6470Valid : 1,
	word88Valid : 1,
	/* reserved */
	: 13;
	uint16_t oldCurCylinderCount;
	uint16_t oldCurHeadCount;
	uint16_t oldCurSecsPerTrack;
	uint32_t oldCurCapacity;	/* in sectors */
	/* current seting for number of sectors that can be transferred per interrupt on R/W multiple
	 * commands */
	uint16_t curmaxSecsPerIntrpt : 8,
	/* multiple sector setting is valid */
	multipleSecsValid : 1,
	/* reserved */
	: 7;
	/* total number of user addressable sectors (LBA mode only) */
	uint32_t userSectorCount;
	uint8_t oldswDMAActive;
	uint8_t oldswDMASupported;
	uint16_t mwDMAMode0Supp : 1,
	mwDMAMode1Supp : 1,
	mwDMAMode2Supp : 1,
	/* reserved */
	: 5,
	mwDMAMode0Sel : 1,
	mwDMAMode1Sel : 1,
	mwDMAMode2Sel : 1,
	/* reserved */
	: 5;
	uint8_t supportedPIOModes;
	/* reserved */
	uint8_t : 8;
	uint16_t minMwDMATransTimePerWord;	/* in nanoseconds */
	uint16_t recMwDMATransTime;
	uint16_t minPIOTransTime;
	uint16_t minPIOTransTimeIncCtrlFlow;
	/* reserved / uninteresting */
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	uint16_t : 16;
	union {
		struct {
			uint16_t : 1,
			ata1 : 1,
			ata2 : 1,
			ata3 : 1,
			ata4 : 1,
			ata5 : 1,
			ata6 : 1,
			ata7 : 1,
			: 8;
		} A_PACKED bits;
		uint16_t raw;
	} majorVersion;
	uint16_t minorVersion;
	struct {
		uint16_t smart : 1,
		securityMode : 1,
		removableMedia : 1,
		powerManagement : 1,
		packet : 1,
		writeCache : 1,
		lookAhead : 1,
		releaseInt : 1,
		serviceInt : 1,
		deviceReset : 1,
		hostProtArea : 1,
		: 1,
		writeBuffer : 1,
		readBuffer : 1,
		nop : 1,
		: 1;

		uint16_t downloadMicrocode : 1,
		rwDMAQueued : 1,
		cfa : 1,
		apm : 1,
		/* removable media status notification */
		removableMediaSN : 1,
		powerupStandby : 1,
		setFeaturesSpinup : 1,
		: 1,
		setMaxSecurity : 1,
		autoAcousticMngmnt: 1,
		lba48 : 1,
		devConfigOverlay : 1,
		flushCache : 1,
		flushCacheExt : 1,
		: 2;
	} A_PACKED features;
	uint16_t reserved[172];
} A_PACKED sATAIdentify;

typedef struct sATAController sATAController;
typedef struct sATADevice sATADevice;
typedef bool (*fReadWrite)(sATADevice *device,uint op,void *buffer,uint64_t lba,size_t secSize,
		size_t secCount);

struct sATADevice {
	/* the identifier; 0-3; bit0 set means slave */
	uchar id;
	/* whether the device exists and we can use it */
	uchar present;
	/* master / slave */
	uchar slaveBit;
	/* the sector-size */
	size_t secSize;
	/* the ata-controller to which the device belongs */
	sATAController *ctrl;
	/* handler-function for reading / writing */
	fReadWrite rwHandler;
	/* various informations we got via IDENTIFY-command */
	sATAIdentify info;
	/* the partition-table */
	sPartition partTable[PARTITION_COUNT];
};

/* physical region daskriptor */
typedef struct {
	uint32_t buffer;
	uint16_t byteCount;
	uint16_t : 15;
	uint16_t last : 1;
} A_PACKED sPRD;

/* the controller is declared here, because otherwise device.h needs controller.h and the other way
 * around */
struct sATAController {
	uchar id;
	uchar useIrq;
	uchar useDma;
	/* I/O-ports for the controllers */
	uint16_t portBase;
	/* I/O-ports for bus-mastering */
	uint16_t bmrBase;
	int irq;
	int irqsem;
	sPRD *dma_prdt_phys;
	sPRD *dma_prdt_virt;
	void *dma_buf_phys;
	void *dma_buf_virt;
	sATADevice devices[2];
};

/**
 * Inits the given device
 *
 * @param device the device
 */
void device_init(sATADevice *device);

#if DEBUGGING

/**
 * Prints information about the given device
 */
void device_dbg_printInfo(sATADevice *device);

#endif
