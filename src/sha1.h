/*  $Id: sha1.h,v 1.2 2023/05/18 14:33:02 atl Exp $  */
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

typedef struct {
  unsigned int H[5];
  unsigned int W[80];
  int lenW;
  unsigned int sizeHi,sizeLo;
} SHA_CTX;

void SHA1_Init(SHA_CTX *ctx);
void SHA1_Update(SHA_CTX *ctx, void *dataIn, int len);
void SHA1_Final(unsigned char hashout[20], SHA_CTX *ctx);
