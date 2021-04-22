/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef _OSD_IO_H_
#define _OSD_IO_H_

#include <asm/arch/io.h>
#include <asm/arch/secure_apb.h>


#define REG_OFFSET_CBUS(reg)            ((reg << 2))
#define REG_OFFSET_VCBUS(reg)           ((reg << 2))

#define REG_ADDR_CBUS(reg)              (REG_BASE_CBUS + REG_OFFSET_CBUS(reg))
#define REG_ADDR_VCBUS(reg)             (REG_BASE_VCBUS + REG_OFFSET_VCBUS(reg))

static inline u32 osd_cbus_read(u32 reg)
{
	return (*(volatile unsigned int *)REG_ADDR_CBUS(reg));
}

static inline void osd_cbus_write(u32 reg,
				  const u32 val)
{
	*(volatile unsigned int *)REG_ADDR_CBUS(reg) = (val);
}


static inline u32 osd_reg_read(u32 reg)
{
	return (*(volatile unsigned int *)REG_ADDR_VCBUS(reg));
}

static inline void osd_reg_write(u32 reg,
				 const u32 val)
{
	*(volatile unsigned int *)REG_ADDR_VCBUS(reg) = (val);
}

static inline void osd_reg_set_mask(u32 reg,
				    const u32 mask)
{
	osd_reg_write(reg, (osd_reg_read(reg) | (mask)));
}

static inline void osd_reg_clr_mask(u32 reg,
				    const u32 mask)
{
	osd_reg_write(reg, (osd_reg_read(reg) & (~(mask))));
}

static inline void osd_reg_set_bits(u32 reg,
				    const u32 value,
				    const u32 start,
				    const u32 len)
{
	osd_reg_write(reg, ((osd_reg_read(reg) &
			     ~(((1L << (len)) - 1) << (start))) |
			    (((value) & ((1L << (len)) - 1)) << (start))));
}

#define VSYNCOSD_RD_MPEG_REG(reg) osd_reg_read(reg)
#define VSYNCOSD_WR_MPEG_REG(reg, val) osd_reg_write(reg, val)
#define VSYNCOSD_WR_MPEG_REG_BITS(reg, val, start, len) \
	osd_reg_set_bits(reg, val, start, len)
#define VSYNCOSD_SET_MPEG_REG_MASK(reg, mask) osd_reg_set_mask(reg, mask)
#define VSYNCOSD_CLR_MPEG_REG_MASK(reg, mask) osd_reg_clr_mask(reg, mask)

#endif
