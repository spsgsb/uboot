// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Copyright (c) 2019 Amlogic, Inc. All rights reserved.
 */

//#include <endian.h>
#include <stdarg.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>

#include <libavb/avb_sysdeps.h>

int avb_memcmp(const void* src1, const void* src2, size_t n) {
  return memcmp(src1, src2, n);
}

void* avb_memcpy(void* dest, const void* src, size_t n) {
  return memcpy(dest, src, n);
}

void* avb_memset(void* dest, const int c, size_t n) {
  return memset(dest, c, n);
}

int avb_strcmp(const char* s1, const char* s2) {
  return strcmp(s1, s2);
}

size_t avb_strlen(const char* str) {
  return strlen(str);
}

void avb_abort(void) {
  panic ("avb abort\n");
  //abort();
}

void avb_print(const char* message) {
  printf("%s", message);
}

void avb_printv(const char* message, ...) {
  va_list ap;
  const char* m;

  va_start(ap, message);
  for (m = message; m != NULL; m = va_arg(ap, const char*)) {
    printf("%s", m);
  }
  va_end(ap);
}

void* avb_malloc_(size_t size) {
  return malloc(size);
}

void avb_free(void* ptr) {
  free(ptr);
}

uint32_t avb_div_by_10(uint64_t* dividend) {
  uint32_t rem = (uint32_t)(*dividend % 10);
  *dividend /= 10;
  return rem;
}
