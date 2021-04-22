/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef _FB_NAND_H_
#define _FB_NAND_H_
void fb_nand_flash_write(const char *cmd, void *download_buffer,
			unsigned int download_bytes);
void fb_nand_erase(const char *cmd, void *download_buffer);
#endif /*_FB_NAND_H_*/

