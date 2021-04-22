// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

#include <libavb/avb_version.h>

#define AVB_QUOTE(str) #str
#define AVB_EXPAND_AND_QUOTE(str) AVB_QUOTE(str)

/* Keep in sync with get_release_string() in avbtool. */
const char* avb_version_string(void) {
  return AVB_EXPAND_AND_QUOTE(AVB_VERSION_MAJOR) "." AVB_EXPAND_AND_QUOTE(
      AVB_VERSION_MINOR) "." AVB_EXPAND_AND_QUOTE(AVB_VERSION_SUB);
}
