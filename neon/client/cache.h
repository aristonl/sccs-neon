/*
 * NEON - A simple project revision tracker
 *
 * Copyright (c) 2023 Ariston Lorenzo.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CACHE_H
#define CACHE_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
#ifndef lint
static char rcsid[] = "@(#) $Id: cache.h,v 1.1.1.1 2024/05/30 13:38:04 atl Exp $"
#endif /* not lint */
#endif

/*
 * Structs for the project cache
 *
 * Based off of GIT.
 */

#define CACHE_SIGNATURE 0x43415348 /* "CASH" :^) */
struct cache_header {
	unsigned int sig; /* Signature */
	unsigned int ver; /* Version */
	unsigned int ent; /* Entries */
};

#define SHA_DIGEST_LENGTH 20

struct cache_entry {
	unsigned short namelen;
	char *name;
	unsigned int mode;
	unsigned int size;
	unsigned char sha[SHA_DIGEST_LENGTH];
};

int write_cache(char* path);
int read_cache(void);

int verify_header(void);

#endif
