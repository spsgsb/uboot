/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __EFUSE_H
#define __EFUSE_H

#include <config.h>
#include <common.h>

/* efuse HAL_API arg */
struct efuse_hal_api_arg {
	unsigned int cmd;		/* R/W */
	unsigned int offset;
	unsigned int size;
	unsigned long buffer_phy;
	unsigned long retcnt_phy;
};


#define EFUSE_BYTES				256   /* (EFUSE_BITS/8) */

#define EFUSE_HAL_API_READ	0
#define EFUSE_HAL_API_WRITE 1
#define EFUSE_HAL_API_WRITE_PATTERN 2
#define EFUSE_HAL_API_USER_MAX 3

#define EFUSE_USER_MASK            (0x1 << 16)
#define EFUSE_THERMAL_MASK         (0x1 << 17)
#define EFUSE_THERMAL_VERFLAG_MASK (0x1 << 18)
#define EFUSE_ENCRYPT_MASK         (0x1 << 19)

//#define ASSIST_HW_REV                              0x1f53

int efuse_read_usr(char *buf, size_t count, loff_t *ppos);
int efuse_write_usr(char *buf, size_t count, loff_t *ppos);
uint32_t efuse_get_max(void);
ssize_t efuse_read(char *buf, size_t count, loff_t *ppos);
ssize_t efuse_write(const char *buf, size_t count, loff_t *ppos);

int32_t meson_trustzone_efuse(struct efuse_hal_api_arg *arg);
int32_t meson_trustzone_efuse_get_max(struct efuse_hal_api_arg *arg);

#endif

