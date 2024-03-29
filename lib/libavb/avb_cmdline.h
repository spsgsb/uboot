/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#ifdef AVB_INSIDE_LIBAVB_H
#error "You can't include avb_sha.h in the public header libavb.h."
#endif

#ifndef AVB_COMPILATION
#error "Never include this file, it may only be used from internal avb code."
#endif

#ifndef AVB_CMDLINE_H_
#define AVB_CMDLINE_H_

#include <libavb/avb_ops.h>
#include <libavb/avb_slot_verify.h>

/* Maximum allow length (in bytes) of a partition name, including
 * ab_suffix.
 */
#define AVB_PART_NAME_MAX_SIZE 32

#define AVB_MAX_NUM_CMDLINE_SUBST 10

/* Holds information about command-line substitutions. */
typedef struct AvbCmdlineSubstList {
  size_t size;
  char* tokens[AVB_MAX_NUM_CMDLINE_SUBST];
  char* values[AVB_MAX_NUM_CMDLINE_SUBST];
} AvbCmdlineSubstList;

/* Substitutes all variables (e.g. $(ANDROID_SYSTEM_PARTUUID)) with
 * values. Returns NULL on OOM, otherwise the cmdline with values
 * replaced.
 */
char* avb_sub_cmdline(AvbOps* ops,
                      const char* cmdline,
                      const char* ab_suffix,
                      bool using_boot_for_vbmeta,
                      const AvbCmdlineSubstList* additional_substitutions);

AvbSlotVerifyResult avb_append_options(
    AvbOps* ops,
    AvbSlotVerifyData* slot_data,
    AvbVBMetaImageHeader* toplevel_vbmeta,
    AvbAlgorithmType algorithm_type,
    AvbHashtreeErrorMode hashtree_error_mode);

/* Allocates and initializes a new command line substitution list. Free with
 * |avb_free_cmdline_subst_list|.
 */
AvbCmdlineSubstList* avb_new_cmdline_subst_list(void);

/* Use this instead of |avb_free| to deallocate a AvbCmdlineSubstList. */
void avb_free_cmdline_subst_list(AvbCmdlineSubstList* cmdline_subst);

/* Adds a hashtree root digest to be substituted in $(AVB_*_ROOT_DIGEST)
 * variables. The partition name differentiates the variable. For example, if
 * |part_name| is "foo" then $(AVB_FOO_ROOT_DIGEST) will be substituted with the
 * hex encoding of the digest. The substitution will be added to
 * |out_cmdline_subst|. Returns AVB_SLOT_VERIFY_RESULT_OK on success.
 */
AvbSlotVerifyResult avb_add_root_digest_substitution(
    const char* part_name,
    const uint8_t* digest,
    size_t digest_size,
    AvbCmdlineSubstList* out_cmdline_subst);

#endif
