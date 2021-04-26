/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef __VPU_DUMMY_REG_H__
#define __VPU_DUMMY_REG_H__
#include <asm/arch/io.h>
#include <asm/arch/secure_apb.h>

#ifndef HHI_GP_PLL_CNTL
#define HHI_GP_PLL_CNTL                            (0xc1100000 + (0x1010 << 2))
#endif

#ifndef HHI_GP1_PLL_CNTL
#define HHI_GP1_PLL_CNTL                           (0xc883c000 + (0x16 << 2))
#endif
#ifndef HHI_GP1_PLL_CNTL2
#define HHI_GP1_PLL_CNTL2                          (0xc883c000 + (0x17 << 2))
#endif
#ifndef HHI_GP1_PLL_CNTL3
#define HHI_GP1_PLL_CNTL3                          (0xc883c000 + (0x18 << 2))
#endif
#ifndef HHI_GP1_PLL_CNTL4
#define HHI_GP1_PLL_CNTL4                          (0xc883c000 + (0x19 << 2))
#endif

#ifndef HHI_VPU_MEM_PD_REG2
#define HHI_VPU_MEM_PD_REG2                        (0xff63c000 + (0x4d << 2))
#endif
#ifndef HHI_VPU_MEM_PD_REG3
#define HHI_VPU_MEM_PD_REG3                        (0xff63c000 + (0x4e << 2))
#endif
#ifndef HHI_VPU_MEM_PD_REG4
#define HHI_VPU_MEM_PD_REG4                        (0xff63c000 + (0x4c << 2))
#endif

#ifndef DOLBY_TV_CLKGATE_CTRL
#define DOLBY_TV_CLKGATE_CTRL                      (0x33f1)
#endif
#ifndef DOLBY_CORE1_CLKGATE_CTRL
#define DOLBY_CORE1_CLKGATE_CTRL                   (0x33f2)
#endif
#ifndef DOLBY_CORE2A_CLKGATE_CTRL
#define DOLBY_CORE2A_CLKGATE_CTRL                  (0x3432)
#endif
#ifndef DOLBY_CORE3_CLKGATE_CTRL
#define DOLBY_CORE3_CLKGATE_CTRL                   (0x36f0)
#endif


#endif
