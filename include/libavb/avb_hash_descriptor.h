/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#if !defined(AVB_INSIDE_LIBAVB_H) && !defined(AVB_COMPILATION)
#error "Never include this file directly, include libavb.h instead."
#endif

#ifndef AVB_HASH_DESCRIPTOR_H_
#define AVB_HASH_DESCRIPTOR_H_

#include "avb_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for hash descriptors.
 *
 * AVB_HASH_DESCRIPTOR_FLAGS_DO_NOT_USE_AB: Do not apply the default A/B
 *   partition logic to this partition. This is intentionally a negative boolean
 *   because A/B should be both the default and most used in practice.
 */
typedef enum {
  AVB_HASH_DESCRIPTOR_FLAGS_DO_NOT_USE_AB = (1 << 0),
} AvbHashDescriptorFlags;

/* A descriptor containing information about hash for an image.
 *
 * This descriptor is typically used for boot partitions to verify the
 * entire kernel+initramfs image before executing it.
 *
 * Following this struct are |partition_name_len| bytes of the
 * partition name (UTF-8 encoded), |salt_len| bytes of salt, and then
 * |digest_len| bytes of the digest.
 *
 * The |reserved| field is for future expansion and must be set to NUL
 * bytes.
 *
 * Changes in v1.1:
 *   - flags field is added which supports AVB_HASH_DESCRIPTOR_FLAGS_USE_AB
 *   - digest_len may be zero, which indicates the use of a persistent digest
 */
typedef struct AvbHashDescriptor {
  AvbDescriptor parent_descriptor;
  uint64_t image_size;
  uint8_t hash_algorithm[32];
  uint32_t partition_name_len;
  uint32_t salt_len;
  uint32_t digest_len;
  uint32_t flags;
  uint8_t reserved[60];
} AVB_ATTR_PACKED AvbHashDescriptor;

/* Copies |src| to |dest| and validates, byte-swapping fields in the
 * process if needed. Returns true if valid, false if invalid.
 *
 * Data following the struct is not validated nor copied.
 */
bool avb_hash_descriptor_validate_and_byteswap(const AvbHashDescriptor* src,
                                               AvbHashDescriptor* dest)
    AVB_ATTR_WARN_UNUSED_RESULT;

#ifdef __cplusplus
}
#endif

#endif /* AVB_HASH_DESCRIPTOR_H_ */
