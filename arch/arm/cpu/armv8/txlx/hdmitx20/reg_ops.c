// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <common.h>
#include <asm/arch/io.h>
#include <asm/io.h>
#include "mach_reg.h"
#include "hdmitx_reg.h"
#include <amlogic/hdmi.h>

static int dbg_en;

void hd_write_reg(unsigned long addr, unsigned long val)
{
	writel(val, addr);
	if (dbg_en)
		printk("W: 0x%08lx  0x%08lx %s 0x%08lx\n", addr, val, (val == hd_read_reg(addr)) ? "==" : "!=", hd_read_reg(addr));
}

unsigned long hd_read_reg(unsigned long addr)
{
	unsigned long val = 0;
	val = readl(addr);
	if (dbg_en)
		printk("R: 0x%08lx   0x%08lx\n", addr, val);
	return val;
}

void hd_set_reg_bits(unsigned long addr, unsigned long value,
	unsigned long offset, unsigned long len)
{
	unsigned long data = 0;

	data = hd_read_reg(addr);
	data &= ~(((1L << len) - 1) << offset);
	data |= (value & ((1L << len) - 1)) << offset;
	hd_write_reg(addr, data);
}

#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

unsigned int hdmitx_rd_reg(unsigned int addr)
{
	unsigned long offset = (addr & DWC_OFFSET_MASK) >> 24;
	unsigned int data;
	register long x0 asm("x0") = 0x82000018;
	register long x1 asm("x1") = (unsigned long)addr;

	asm volatile(
		__asmeq("%0", "x0")
		__asmeq("%1", "x1")
		"smc #0\n"
		: "+r"(x0) : "r"(x1)
	);
	data = (unsigned)(x0&0xffffffff);
	if (dbg_en)
		pr_info("%s rd[0x%x] 0x%x\n", offset ? "DWC" : "TOP",
			addr, data);
	return data;
}

void hdmitx_wr_reg(unsigned int addr, unsigned int data)
{
	unsigned long offset = (addr & DWC_OFFSET_MASK) >> 24;
	register long x0 asm("x0") = 0x82000019;
	register long x1 asm("x1") = (unsigned long)addr;
	register long x2 asm("x2") = data;

	asm volatile(
		__asmeq("%0", "x0")
		__asmeq("%1", "x1")
		__asmeq("%2", "x2")
		"smc #0\n"
		: : "r"(x0), "r"(x1), "r"(x2)
	);
	if (dbg_en)
		pr_info("%s wr[0x%x] 0x%x\n", offset ? "DWC" : "TOP",
			addr, data);
}

void hdmitx_set_reg_bits(unsigned int addr, unsigned int value,
	unsigned int offset, unsigned int len)
{
	unsigned int data32 = 0;

	data32 = hdmitx_rd_reg(addr);
	data32 &= ~(((1 << len) - 1) << offset);
	data32 |= (value & ((1 << len) - 1)) << offset;
	hdmitx_wr_reg(addr, data32);
}

void hdmitx_poll_reg(unsigned int addr, unsigned int val, unsigned long timeout)
{
	udelay(2000);
	if (!(hdmitx_rd_reg(addr) & val))
		pr_info("hdmitx poll:0x%x  val:0x%x t=%dms timeout\n",
			addr, val, 2000);
}

void hdmitx_rd_check_reg (unsigned long addr, unsigned long exp_data, unsigned long mask)
{
    unsigned long rd_data;
    rd_data = hdmitx_rd_reg(addr);
    if ((rd_data | mask) != (exp_data | mask)) {
        pr_info("HDMITX-DWC addr=0x%04x rd_data=0x%02x\n", (unsigned int)addr, (unsigned int)rd_data);
        pr_info("Error: HDMITX-DWC exp_data=0x%02x mask=0x%02x\n", (unsigned int)exp_data, (unsigned int)mask);
    }
}
