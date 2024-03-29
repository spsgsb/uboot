/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __ARCH_ARM_MESON_PCI_H_U_BOOT__
#define __ARCH_ARM_MESON_PCI_H_U_BOOT__

#include <common.h>
#include <asm/types.h>

void amlogic_pcie_init_reset_pin(int pcie_dev);
void amlogic_pcie_disable(void);
#endif

