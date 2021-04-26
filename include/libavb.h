/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifndef LIBAVB_H_
#define LIBAVB_H_

/* The AVB_INSIDE_LIBAVB_H preprocessor symbol is used to enforce
 * library users to include only this file. All public interfaces, and
 * only public interfaces, must be included here.
 */

#define AVB_INSIDE_LIBAVB_H
#include "libavb/avb_chain_partition_descriptor.h"
#include "libavb/avb_crypto.h"
#include "libavb/avb_descriptor.h"
#include "libavb/avb_footer.h"
#include "libavb/avb_hash_descriptor.h"
#include "libavb/avb_hashtree_descriptor.h"
#include "libavb/avb_kernel_cmdline_descriptor.h"
#include "libavb/avb_ops.h"
#include "libavb/avb_property_descriptor.h"
#include "libavb/avb_slot_verify.h"
#include "libavb/avb_sysdeps.h"
#include "libavb/avb_util.h"
#include "libavb/avb_vbmeta_image.h"
#include "libavb/avb_version.h"
#undef AVB_INSIDE_LIBAVB_H

int avb_verify(AvbSlotVerifyData** out_data);
int is_device_unlocked(void);
#endif /* LIBAVB_H_ */
