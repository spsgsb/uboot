// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include "power.c"

/* bl2 customer code */
void board_init(void)
{
#ifndef CONFIG_PXP_EMULATOR
	power_init(0);
#endif
}