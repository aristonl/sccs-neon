/*  $Id: sha1.c,v 1.1.1.1 2024/05/30 13:38:05 atl Exp $  */
/*
 * NEON - A simple project revision tracker
 *
 * Copyright (c) 1995-1999 Cryptography Research, Inc.
 * Copyright (c) 2023 Ariston Lorenzo.
 *
 * This file was created by Paul Kocher and is distributed under
 * the GNU General Public License version 2. See COPYING.SHA1 in
 * the NEON source tree for more information.
 */

#include "sha1.h"

static void shaHashBlock(SHA_CTX *ctx);

void SHA1_Init(SHA_CTX *ctx) {
  int i;

  ctx->lenW = 0;
  ctx->sizeHi = ctx->sizeLo = 0;

  /* Initialize H with the magic constants (see FIPS180 for constants)
   */
  ctx->H[0] = 0x67452301;
  ctx->H[1] = 0xefcdab89;
  ctx->H[2] = 0x98badcfe;
  ctx->H[3] = 0x10325476;
  ctx->H[4] = 0xc3d2e1f0;

  for (i = 0; i < 80; i++)
    ctx->W[i] = 0;
}


void SHA1_Update(SHA_CTX *ctx, void *_dataIn, int len) {
  unsigned char *dataIn = _dataIn;
  int i;

  /* Read the data into W and process blocks as they get full
   */
  for (i = 0; i < len; i++) {
    ctx->W[ctx->lenW / 4] <<= 8;
    ctx->W[ctx->lenW / 4] |= (unsigned int)dataIn[i];
    if ((++ctx->lenW) % 64 == 0) {
      shaHashBlock(ctx);
      ctx->lenW = 0;
    }
    ctx->sizeLo += 8;
    ctx->sizeHi += (ctx->sizeLo < 8);
  }
}


void SHA1_Final(unsigned char hashout[20], SHA_CTX *ctx) {
  unsigned char pad0x80 = 0x80;
  unsigned char pad0x00 = 0x00;
  unsigned char padlen[8];
  int i;

  /* Pad with a binary 1 (e.g. 0x80), then zeroes, then length
   */
  padlen[0] = (unsigned char)((ctx->sizeHi >> 24) & 255);
  padlen[1] = (unsigned char)((ctx->sizeHi >> 16) & 255);
  padlen[2] = (unsigned char)((ctx->sizeHi >> 8) & 255);
  padlen[3] = (unsigned char)((ctx->sizeHi >> 0) & 255);
  padlen[4] = (unsigned char)((ctx->sizeLo >> 24) & 255);
  padlen[5] = (unsigned char)((ctx->sizeLo >> 16) & 255);
  padlen[6] = (unsigned char)((ctx->sizeLo >> 8) & 255);
  padlen[7] = (unsigned char)((ctx->sizeLo >> 0) & 255);
  SHA1_Update(ctx, &pad0x80, 1);
  while (ctx->lenW != 56)
    SHA1_Update(ctx, &pad0x00, 1);
  SHA1_Update(ctx, padlen, 8);

  /* Output hash
   */
  for (i = 0; i < 20; i++) {
    hashout[i] = (unsigned char)(ctx->H[i / 4] >> 24);
    ctx->H[i / 4] <<= 8;
  }

  /*
   *  Re-initialize the context (also zeroizes contents)
   */
  SHA1_Init(ctx);
}


#define SHA_ROT(X,n) (((X) << (n)) | ((X) >> (32-(n))))

static void shaHashBlock(SHA_CTX *ctx) {
  int t;
  unsigned int A,B,C,D,E,TEMP;

  for (t = 16; t <= 79; t++)
    ctx->W[t] =
      SHA_ROT(ctx->W[t-3] ^ ctx->W[t-8] ^ ctx->W[t-14] ^ ctx->W[t-16], 1);

  A = ctx->H[0];
  B = ctx->H[1];
  C = ctx->H[2];
  D = ctx->H[3];
  E = ctx->H[4];

  for (t = 0; t <= 19; t++) {
    TEMP = SHA_ROT(A,5) + (((C^D)&B)^D)     + E + ctx->W[t] + 0x5a827999;
    E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
  }
  for (t = 20; t <= 39; t++) {
    TEMP = SHA_ROT(A,5) + (B^C^D)           + E + ctx->W[t] + 0x6ed9eba1;
    E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
  }
  for (t = 40; t <= 59; t++) {
    TEMP = SHA_ROT(A,5) + ((B&C)|(D&(B|C))) + E + ctx->W[t] + 0x8f1bbcdc;
    E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
  }
  for (t = 60; t <= 79; t++) {
    TEMP = SHA_ROT(A,5) + (B^C^D)           + E + ctx->W[t] + 0xca62c1d6;
    E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
  }

  ctx->H[0] += A;
  ctx->H[1] += B;
  ctx->H[2] += C;
  ctx->H[3] += D;
  ctx->H[4] += E;
}

