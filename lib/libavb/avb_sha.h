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

#ifndef AVB_SHA_H_
#define AVB_SHA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libavb/avb_crypto.h>
#include <libavb/avb_sysdeps.h>

/* Block size in bytes of a SHA-256 digest. */
#define AVB_SHA256_BLOCK_SIZE 64


/* Block size in bytes of a SHA-512 digest. */
#define AVB_SHA512_BLOCK_SIZE 128

/* Data structure used for SHA-256. */
typedef struct {
  uint32_t h[8];
  uint32_t tot_len;
  uint32_t len;
  uint8_t block[2 * AVB_SHA256_BLOCK_SIZE];
  uint8_t buf[AVB_SHA256_DIGEST_SIZE]; /* Used for storing the final digest. */
} AvbSHA256Ctx;

/* Data structure used for SHA-512. */
typedef struct {
  uint64_t h[8];
  uint32_t tot_len;
  uint32_t len;
  uint8_t block[2 * AVB_SHA512_BLOCK_SIZE];
  uint8_t buf[AVB_SHA512_DIGEST_SIZE]; /* Used for storing the final digest. */
} AvbSHA512Ctx;

/* Initializes the SHA-256 context. */
void avb_sha256_init(AvbSHA256Ctx* ctx);

/* Updates the SHA-256 context with |len| bytes from |data|. */
void avb_sha256_update(AvbSHA256Ctx* ctx, const uint8_t* data, uint32_t len);

/* Returns the SHA-256 digest. */
uint8_t* avb_sha256_final(AvbSHA256Ctx* ctx) AVB_ATTR_WARN_UNUSED_RESULT;

/* Initializes the SHA-512 context. */
void avb_sha512_init(AvbSHA512Ctx* ctx);

/* Updates the SHA-512 context with |len| bytes from |data|. */
void avb_sha512_update(AvbSHA512Ctx* ctx, const uint8_t* data, uint32_t len);

/* Returns the SHA-512 digest. */
uint8_t* avb_sha512_final(AvbSHA512Ctx* ctx) AVB_ATTR_WARN_UNUSED_RESULT;

#ifdef __cplusplus
}
#endif

#endif /* AVB_SHA_H_ */
