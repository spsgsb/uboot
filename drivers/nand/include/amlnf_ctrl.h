/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef AMLNAND_PHYDEV_H_INCLUDED
#define AMLNAND_PHYDEV_H_INCLUDED

#include "amlnf_dev.h"

/***nand CE/RB pinmux setting***/
#define	AML_NAND_CE0	0xe
#define	AML_NAND_CE1	0xd
#define	AML_NAND_CE2	0xb
#define	AML_NAND_CE3	0x7

#define	CE_PAD_DEFAULT	\
	((AML_NAND_CE0) | \
	(AML_NAND_CE1 << 4) | \
	(AML_NAND_CE2 << 8) | \
	(AML_NAND_CE3 << 12))

/*
#define RB_PAD_DEFAULT	((AML_NAND_CE0) | (AML_NAND_CE1 << 4))
*/

#define	RB_PAD_DEFAULT	(AML_NAND_CE0)
#define	MAX_CHIP_NUM	4

#define	PER_INFO_BYTE	8

/***nand device option***/
#define	DEV_SLC_MODE		(1<<0)
#define	DEV_SERIAL_CHIP_MODE	(0<<1)
#define	DEV_MULTI_CHIP_MODE	(1<<1)

#define	DEV_MULTI_PLANE_MODE	(1<<2)
#define	DEV_SINGLE_PLANE_MODE	(0<<2)

#define	DEV_USE_SHAREPAGE_MODE	(1<<4)

#define	DEV_ECC_SOFT_MODE	(1<<3)
#define	DEV_ECC_HW_MODE		(0<<3)

#define	NAND_SHUT_DOWN		(1 << 16)

#define	NAND_CODE_OPTION	(DEV_MULTI_PLANE_MODE | DEV_MULTI_CHIP_MODE | DEV_USE_SHAREPAGE_MODE)
#define	NAND_DATA_OPTION	(DEV_MULTI_PLANE_MODE | DEV_MULTI_CHIP_MODE | DEV_USE_SHAREPAGE_MODE)

/*
 * Status for nand chip
 */
enum chip_state_t {
	CHIP_READY,
	CHIP_STATUS,
	CHIP_ERASING,
	CHIP_ERASE_SUSPENDING,
	CHIP_ERASE_SUSPENDED,
	CHIP_WRITING,
	CHIP_WRITE_SUSPENDING,
	CHIP_WRITE_SUSPENDED,
	CHIP_PM_SUSPENDED,
	CHIP_SYNCING,
	CHIP_UNLOADING,
	CHIP_LOCKING,
	CHIP_UNLOCKING,
	CHIP_POINT,
	CHIP_SHUTDOWN,
	CHIP_READING,
	CHIP_RESETING,
	CHIP_PREPARING_ERASE,
	CHIP_VERIFYING_ERASE,
	CHIP_UNKNOWN
};


extern int nandphy_init(u32 flag);
extern int amlphy_prepare(u32 flag);

#endif
